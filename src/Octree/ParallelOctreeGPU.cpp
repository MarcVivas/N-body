#include "ParallelOctreeGPU.h"
#include <cstdint>
#include <memory>
#include <omp.h>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include "Helpers.h"
#include "ParticleSystem.h"
#include <chrono>

using helpers::log;
using helpers::ipow;

ParallelOctreeGPU::ParallelOctreeGPU(const int n){
    this->totalParticles = n;

	initComputeShaders();

    this->maxDepth = -1;
    this->fatherMaxNodes = 0;

    int i = 0;
    while (maxDepth < 8) {
        const int tmp = fatherMaxNodes + ipow(8, i);
        fatherMaxNodes = tmp;
        maxDepth++;
        if (static_cast<float>(n) / static_cast<float>(ipow(8, i)) < 2.f) break;
        i++;
    }

    this->totalTasks = ipow(8, maxDepth);

	log("Total tasks: " + std::to_string(totalTasks), "Father max nodes:", fatherMaxNodes, "Max depth:", maxDepth,
        "MAX NODES:", getMaxNodes());




    initSSBOs(n);

}

void ParallelOctreeGPU::initComputeShaders() {
    this->expander = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/expandOctree.glsl"));
    this->inserter = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/insertParticles.glsl"));
    this->distributor = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/distributeParticles.glsl"));
    this->resetOctree = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/resetOctree.glsl"));
    this->resetTasks = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/resetTasks.glsl"));
    this->boundingBoxPass1 = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/boundingBoxPass1.glsl"));
    this->boundingBoxPass2 = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/boundingBoxPass2.glsl"));
    this->countParticlesPerTask = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/countParticlesPerTask.glsl"));
    this->prefixSum = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/prefixSum.glsl"));
    this->addBlockSums = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/addBlockSums.glsl"));
    this->propagateFather = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/propagateFatherOctree.glsl"));
    this->pruneFather = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/pruneFather.glsl"));
    this->pruneSubtrees = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/pruneSubtrees.glsl"));
    this->buildBinaryTreeShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/buildBinaryTree.glsl"));
    this->buildOctreeShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/buildOctree.glsl"));
    this->assignParentsAndChildrenShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/assignParentsAndChildren.glsl"));
    this->propagateOctreeShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/propagateOctree.glsl"));
}

void ParallelOctreeGPU::initSSBOs(const int n) {
    const int blockSizeForReduction = 1024;
	intermediateBoundsBufferA.createBufferData(sizeof(glm::vec4) * ((static_cast<unsigned long long>(n) + blockSizeForReduction-1) / blockSizeForReduction) * 2, nullptr, 6, GL_DYNAMIC_DRAW);
    intermediateBoundsBufferB.createBufferData(sizeof(glm::vec4) * ((static_cast<unsigned long long>(n) + blockSizeForReduction-1) / blockSizeForReduction) * 2, nullptr, 7, GL_DYNAMIC_DRAW);
    //nodesBuffer.createBufferData(sizeof(Node) * (getMaxNodes()), nullptr, 5, GL_DYNAMIC_DRAW);
	//particlesOffsetsBuffer.createBufferData(sizeof(glm::ivec2) * n, nullptr, 10, GL_DYNAMIC_DRAW);
	taskParticlesIndexesBuffer.createBufferData(sizeof(int) * n, nullptr, 12, GL_DYNAMIC_DRAW);
	subTreeNodeCountsBuffer.createBufferData(sizeof(int) * this->totalTasks, nullptr, 14, GL_DYNAMIC_DRAW);
	subTreeParentCountsBuffer.createBufferData(sizeof(int) * this->totalTasks, nullptr, 15, GL_DYNAMIC_DRAW);


	gpuNodeBuffer.createBufferData(sizeof(GPUNode) * (getMaxNodes()), nullptr, 5, GL_DYNAMIC_DRAW);
	binaryTreeBuffer.createBufferData(sizeof(glm::ivec3) * (n-1), nullptr, 9, GL_DYNAMIC_DRAW);
	blockSumsBuffer.createBufferData(sizeof(int) * (((n-1) * 2 + (2*1024)-1) / (2*1024)) , nullptr, 11, GL_DYNAMIC_DRAW);
	binaryTreeEdges.createBufferData(sizeof(int) * (n-1) * 2, nullptr, 13, GL_DYNAMIC_DRAW);
	binaryParentsBuffer.createBufferData(sizeof(int) * n-1, nullptr, 8, GL_DYNAMIC_DRAW);
	octreeParentsBuffer.createBufferData(sizeof(int) * n*2, nullptr, 10, GL_DYNAMIC_DRAW);
}

/**
 * Computes the bounding box of the particles
 * Parallel reduction to find the min and max coordinates of the particles
 * O(n)
 * @param p Particles
 */
void ParallelOctreeGPU::computeBoundingBox(ParticleSystem *p) {
    this->boundingBoxPass1->use();
    this->boundingBoxPass1->setInt("numParticles", p->size());

    intermediateBoundsBufferA.bindBufferBase(6);

    constexpr static int BLOCK_SIZE = 1024;
    const static GLuint totalWorkGroups = (p->size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
    glDispatchCompute(totalWorkGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    GLuint currentNumItems = totalWorkGroups;
    OpenGLBuffer* readBuffer = &intermediateBoundsBufferA;
    OpenGLBuffer* writeBuffer = &intermediateBoundsBufferB;

    while (currentNumItems > 1) {
        const int workGroups = (currentNumItems + BLOCK_SIZE - 1) / BLOCK_SIZE;
        this->boundingBoxPass2->use();
        this->boundingBoxPass2->setInt("numInputItems", currentNumItems);

        readBuffer->bindBufferBase(6);
        writeBuffer->bindBufferBase(7);

        glDispatchCompute(workGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        currentNumItems = workGroups;
        std::swap(readBuffer, writeBuffer);
    }

    /*
    After the loop, currentNumItems is 1.
    The 'readBuffer' now holds the ID of the buffer containing the single final min/max pair at index 0.
    Copy this pair from the intermediate buffer to the final output buffer.
    */

    // Set the bounding box of the root node
    // this->resetOctree->use();
    // this->resetOctree->setInt("numParticles", p->size());
    // glDispatchCompute(1, 1, 1); // Just the root node :P
    // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

/**
 * Establishes the bounding box of the root node and resets the octree
 * O(n)
 * @param p Particles
 */
void ParallelOctreeGPU::reset(ParticleSystem* p) {
	this->computeBoundingBox(p);
    this->nodeCount = 0;
    this->parentCount = 0;
}

/*
* Subdivides the octree until the maximum depth is reached
* The octree is expanded in a parallel way level by level
* Not inserting the particles yet
*/
void ParallelOctreeGPU::expandTree() {
    int level = 0;
    bool firstNode = true;
    int workGroups;
    while (level < maxDepth) {
        const int currentNodeCount = nodeCount;
        nodeCount += ipow(8, level);
        expander->use();
        expander->setInt("currentNodeCount", currentNodeCount);
        expander->setInt("nextLevelNode", nodeCount);
        expander->setBool("firstNode", firstNode);
        workGroups = (nodeCount - currentNodeCount + 256 - 1) / 256;
        glDispatchCompute(workGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        level++;
        firstNode = false;
    }
    this->parentCount = this->nodeCount; // The parent count is the same as the node count
    this->nodeCount += ipow(8, level);
}

/**
 * Prepares the tasks for the parallel insertion of particles
 * Each task is one node of the last level of the octree
 * Assigns the particles to the tasks
 * O(n)
 */
void ParallelOctreeGPU::prepareTasks(ParticleSystem* p) {
    int workGroups;

    // ============================================
    // Count particles per task
    countParticlesPerTask->use();
    countParticlesPerTask->setInt("totalTasks", totalTasks);
    countParticlesPerTask->setInt("totalParticles", p->size());
    countParticlesPerTask->setInt("maxDepth", maxDepth);
    workGroups = (p->size() + 256 - 1) / 256;
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	// ============================================

    // ============================================
	// Prefix sum of the particles per task
    prefixSum->use();
    prefixSum->setInt("N", totalTasks);
    prefixSum->setBool("secondPass", false);
    workGroups = (totalTasks + (1024 * 2) - 1) / (1024 * 2);
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    prefixSum->use();
    prefixSum->setInt("N", workGroups);
    prefixSum->setBool("secondPass", true);
    workGroups = (workGroups + (1024 * 2) - 1) / (1024 * 2);
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    addBlockSums->use();
    addBlockSums->setInt("N", totalTasks);
    workGroups = (totalTasks + 1024 - 1) / 1024;
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // ============================================

    // ============================================
    // Distribute the task Particles indexes
	// Store the indexes of the particles of each task in the taskParticlesIndexesBuffer
    distributor->use();
    distributor->setInt("totalParticles", p->size());
    distributor->setInt("totalTasks", totalTasks);
    workGroups = (p->size() + 256 - 1) / 256;
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // ============================================

}

void ParallelOctreeGPU::executeTasks() {

    inserter->use();
    inserter->setInt("totalTasks", totalTasks);
    inserter->setInt("fatherTreeTotalNodes", fatherMaxNodes);
    int workGroups = (totalTasks + 256 - 1) / 256;
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

/**
 * Inserts the particles
 * @param p particles
 */
void ParallelOctreeGPU::insert(ParticleSystem* p) {

    this->buildBinaryTreeShader->use();
    this->buildBinaryTreeShader->setInt("numInternalNodes", p->size()-1);
    glDispatchCompute((p->size()-1 + 255)/ 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto v = binaryTreeBuffer.getDataVector<glm::ivec3>(0, sizeof(glm::ivec3) * (p->size()-1));
    auto v4 = binaryParentsBuffer.getDataVector<int>(0, sizeof(int) * (p->size()-1));

    int i = 0;
    for(auto &s : v){
        log(s.x, s.y, s.z, v4[i]);
        i++;
    }

    auto v2 = binaryTreeEdges.getDataVector<int>(0, sizeof(int) * (p->size()-1)*2);
    log(v2);

    // ============================================
	// Prefix sum of the particles per task
    prefixSum->use();
    prefixSum->setInt("N", (p->size()-1)*2);
    prefixSum->setBool("secondPass", false);
    int workGroups = ((p->size()-1)*2 + (1024 * 2) - 1) / (1024 * 2);
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    prefixSum->use();
    prefixSum->setInt("N", workGroups);
    prefixSum->setBool("secondPass", true);
    workGroups = (workGroups + (1024 * 2) - 1) / (1024 * 2);
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    addBlockSums->use();
    addBlockSums->setInt("N", (p->size()-1)*2);
    workGroups = ((p->size()-1)*2 + 1024 - 1) / 1024;
    glDispatchCompute(workGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // ============================================
    //

    auto v3 = binaryTreeEdges.getDataVector<int>(0, sizeof(int) * (p->size()-1)*2);
    log(v3);

    this->buildOctreeShader->use();
    this->buildOctreeShader->setInt("totalBinaryTreeEdges", (p->size()-1) * 2);
    glDispatchCompute(((p->size()-1) * 2 + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto v5 = gpuNodeBuffer.getDataVector<GPUNode>(0, sizeof(GPUNode) * p->size() * 2);
    for(auto &n : v5){
        log(n.children[0], n.mass, n.centerOfMass.x, n.centerOfMass.y, n.centerOfMass.z);
    }

    this->assignParentsAndChildrenShader->use();
    this->assignParentsAndChildrenShader->setInt("totalBinaryTreeEdges", (p->size()-1) * 2);
    glDispatchCompute(((p->size()-1) * 2 + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto v6 = gpuNodeBuffer.getDataVector<GPUNode>(0, sizeof(GPUNode) * p->size() * 2);
    auto v7 = octreeParentsBuffer.getDataVector<int>(0, sizeof(int) * p->size() * 2);
    for(uint32_t i = 0; i < p->size() * 2; i ++){
        log("Node", i);
        log("Parent", v7[i]);
        log("Total children", v6[i].numChildren);
        log("Mass", v6[i].mass,
            "Center of mass", v6[i].centerOfMass.x, v6[i].centerOfMass.y, v6[i].centerOfMass.z, ")",
            "Min Boundary", v6[i].minBoundary.x, v6[i].minBoundary.y, v6[i].minBoundary.z, ")",
            "Max Boundary", v6[i].maxBoundary.x, v6[i].maxBoundary.y, v6[i].maxBoundary.z, ")"
        );
        for(int j = 0; j < 8; j++){
            log("Child", i, v6[i].children[j]);
        }

    }

    this->propagateOctreeShader->use();
    this->assignParentsAndChildrenShader->setInt("totalBinaryTreeEdges", (p->size()-1) * 2);
    glDispatchCompute(((p->size()-1) * 2 + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto v8 = gpuNodeBuffer.getDataVector<GPUNode>(0, sizeof(GPUNode) * p->size() * 2);
    for(uint32_t i = 0; i < p->size() * 2; i ++){
        log("Node", i);
        log("Total children", v8[i].numChildren);
        log("Mass", v8[i].mass,
            "Center of mass", v8[i].centerOfMass.x, v8[i].centerOfMass.y, v8[i].centerOfMass.z, ")",
            "Min Boundary", v8[i].minBoundary.x, v8[i].minBoundary.y, v8[i].minBoundary.z, ")",
            "Max Boundary", v8[i].maxBoundary.x, v8[i].maxBoundary.y, v8[i].maxBoundary.z, ")"
        );
        for(int j = 0; j < 8; j++){
            log("Child", i, v8[i].children[j]);
        }

    }
}

void ParallelOctreeGPU::prune() {
    // Prune the father octree first
    int level = 0;
    int left = 0;
    int right = ipow(8, level);

    while (level < maxDepth) {
        pruneFather->use();
        pruneFather->setInt("left", left);
        pruneFather->setInt("right", right);
        glDispatchCompute((right - left + 256 - 1) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		left = right; // The left is the right of the previous level
        level++;
		right += ipow(8, level);
    }

   	// Prune the subtrees
	pruneSubtrees->use();
	pruneSubtrees->setInt("totalTasks", totalTasks);
	glDispatchCompute((totalTasks + 256 - 1) / 256, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


}


void ParallelOctreeGPU::propagate() {
    // Propagate
    int level = maxDepth - 1;
    int right = parentCount;
    int processedNumNodes = ipow(8, level);

    while (level >= 0) {
		const int left = parentCount - processedNumNodes;
        propagateFather->use();
		propagateFather->setInt("left", left);
		propagateFather->setInt("right", right);
		glDispatchCompute((right - left + 256 - 1) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        right = left;
        level--;
        processedNumNodes += ipow(8, level);
    }
}

/**
 * Resets the tasks
 */
void ParallelOctreeGPU::resetTasksBuffer() {


	// Reset the tasks
	resetTasks->use();
	resetTasks->setInt("totalTasks", totalTasks);
	glDispatchCompute((totalTasks + 256 - 1) / 256, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

ParallelOctreeGPU::~ParallelOctreeGPU() = default;
