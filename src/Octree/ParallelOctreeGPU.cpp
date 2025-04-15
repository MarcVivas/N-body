#include "ParallelOctreeGPU.h"
#include <omp.h>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include "Helpers.h"
#include <algorithm>    // std::sort
using helpers::log;
using helpers::ipow; 

ParallelOctreeGPU::ParallelOctreeGPU(const int n){
    this->totalParticles = n;

	initComputeShaders();   

    this->maxDepth = -1;
    this->fatherMaxNodes = 0;
    
    int i = 0;
    while (fatherMaxNodes < n && maxDepth < 8) {
        const int tmp = fatherMaxNodes + ipow(8, i);
        if (tmp >= n) break;
        fatherMaxNodes = tmp;
        maxDepth++;
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
}

void ParallelOctreeGPU::initSSBOs(const int n) {
    const int blockSizeForReduction = 1024;
	intermediateBoundsBufferA.createBufferData(sizeof(glm::vec4) * ((static_cast<unsigned long long>(n) + blockSizeForReduction-1) / blockSizeForReduction) * 2, nullptr, 6, GL_DYNAMIC_DRAW);
    intermediateBoundsBufferB.createBufferData(sizeof(glm::vec4) * ((static_cast<unsigned long long>(n) + blockSizeForReduction-1) / blockSizeForReduction) * 2, nullptr, 7, GL_DYNAMIC_DRAW);
	parentsBuffer.createBufferData(sizeof(int) * n*8, nullptr, 8, GL_DYNAMIC_DRAW);
    nodesBuffer.createBufferData(sizeof(Node) * (getMaxNodes()), nullptr, 5, GL_DYNAMIC_DRAW);
    tasksBuffer.createBufferData(sizeof(Task) * totalTasks, nullptr, 9, GL_DYNAMIC_DRAW);
	particlesOffsetsBuffer.createBufferData(sizeof(glm::ivec2) * n, nullptr, 10, GL_DYNAMIC_DRAW);
	blockSumsBuffer.createBufferData(sizeof(int) * ((totalTasks + (2*1024)-1) / (2*1024)) , nullptr, 11, GL_DYNAMIC_DRAW);
	taskParticlesIndexesBuffer.createBufferData(sizeof(int) * n, nullptr, 12, GL_DYNAMIC_DRAW);
	subTreeParentsBuffer.createBufferData(sizeof(int) * getMaxNodes(), nullptr, 13, GL_DYNAMIC_DRAW);
	subTreeNodeCountsBuffer.createBufferData(sizeof(int) * this->totalTasks, nullptr, 14, GL_DYNAMIC_DRAW);
	subTreeParentCountsBuffer.createBufferData(sizeof(int) * this->totalTasks, nullptr, 15, GL_DYNAMIC_DRAW);
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

    const int BLOCK_SIZE = 1024;
    GLuint totalWorkGroups = (p->size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
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
    this->resetOctree->use();
    glDispatchCompute(64, 1, 1); // Just the root node :P
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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

    this->resetTasksBuffer();

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

    // Expand the tree
	this->expandTree();
   
	this->prepareTasks(p);
    
  
	
    this->executeTasks();
    

    
    this->propagate();
    


    this->prune();

    
    

}


void ParallelOctreeGPU::prune() {
    // Prune the father octree first
    int level = 0;
    int left = 0;
    int right = ipow(8, level);
    int processedNumNodes = ipow(8, level);

	
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
