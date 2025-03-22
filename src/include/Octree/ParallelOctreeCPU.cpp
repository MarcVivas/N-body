#include "ParallelOctreeCPU.h"
#include <numeric>
#include <execution>
#include <omp.h>
#include <iostream>
#include <glm/gtx/norm.hpp>


ParallelOctreeCPU::ParallelOctreeCPU(int n): Octree(n) {
    this->numThreads = omp_get_max_threads();
    this->maxDepth = 2;

    delete[] this->nodes;

    this->maxNodesPerSubtree = n < 200 ?  n*8 : n*2;

    this->maxNodes = 0;
    for (int i = 0; i <= maxDepth; i++) {
        maxNodes += std::pow(8, i);
    }


    this->nodeLocks.resize(maxNodes);
    for (int i = 0; i < maxNodes; ++i) {
        omp_init_lock(&nodeLocks[i]);
    }
    this->totalTasks = std::pow(8, maxDepth);

    this->nodes = new Node[this->maxNodesPerSubtree * this->totalTasks + this->maxNodes + n];

    this->tasks = new Task[totalTasks];

    nodeCounts = new int[this->totalTasks];
    subTreeParents = new int[this->totalTasks*this->maxNodesPerSubtree];
    parentCounts = new int[this->totalTasks];

    this->maxParticlesPerTask = n < 200 ? n : n/2;
    taskParticles = new int[this->totalTasks * maxParticlesPerTask];

  

    this->resetArrays();
}

ParallelOctreeCPU::~ParallelOctreeCPU() {
    delete[] tasks;
    delete[] nodeCounts;
    delete[] subTreeParents;
    delete[] parentCounts;
    delete[] taskParticles;

    for (int i = 0; i < maxNodes; ++i) {
        omp_destroy_lock(&nodeLocks[i]);
    }
}

/**
 * Establishes the bounding box of the root node
 * O(n)
 * @param p Particles
 */
void ParallelOctreeCPU::reset(ParticleSystem *p) {
    float min_x = std::numeric_limits<float>::max();
    float min_y = min_x;
    float min_z = min_x;
    float max_x = std::numeric_limits<float>::min();
    float max_y = max_x;
    float max_z = max_x;

    // Find the max and min positions
    for (int i = 0;  i < p->size(); ++i){
        const glm::vec4 pos = p->getPositions()[i];

        if (pos.x < min_x){
            min_x = pos.x;
        }
        if (pos.y < min_y){
            min_y = pos.y;
        }
        if (pos.z < min_z){
            min_z = pos.z;
        }
        if (pos.x > max_x){
            max_x = pos.x;
        }
        if (pos.y > max_y){
            max_y = pos.y;
        }
        if (pos.z > max_z){
            max_z = pos.z;
        }
    }

    // Set the bounding box of the root node
    this->nodes[0].setBoundingBox(glm::vec4(min_x, min_y, min_z, 0.f), glm::vec4(max_x, max_y, max_z, 0.f));

    Node &root = this->nodes[0];
    root.setFirstChild(-1);
    root.setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f));
    this->nodeCount = 1;
    this->parentCount = 0;
    root.setNext(-1);
}

/**
 * Inserts the particles
 * @param p particles
 */
void ParallelOctreeCPU::insert(ParticleSystem *p) {
       
    int root = 0;

    // Subdivide the tree
    while (nodeCount < maxNodes) {
        Octree::subdivide(root);
        root+=1;
    }
 
    // Fill the tasks
    #pragma omp parallel for
    for (int j = 0; j < p->size(); j++) {
        const glm::vec4 pos = p->getPositions()[j];
        int i = 0;
        int depth = 0;
        while(depth < maxDepth) {
            // Go down until you are at max depth
            // Traverse the tree
            i = this->getNextNode(pos, i);
            depth ++;
        }
        omp_set_lock(&nodeLocks[i]);  // Lock only for this node
        taskParticles[i%totalTasks * maxParticlesPerTask + this->tasks[i%totalTasks].totalParticles] = j;
        this->tasks[i%totalTasks].totalParticles++;
        this->tasks[i%totalTasks].root = i;
        omp_unset_lock(&nodeLocks[i]);  // Unlock after insertion
    }
      
    // Distribute work among threads
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < totalTasks; ++i) {
        // Execute task
        Task &task = this->tasks[i];
        if (task.root < 0) continue;
        
        int root = getSubtree(i);
        Node &oct = this->nodes[root];


        // Reset octrees
        oct.setBoundingBox(this->nodes[task.root].getMinBoundary(), this->nodes[task.root].getMaxBoundary());
        oct.setFirstChild(-1);
        oct.setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f));
        nodeCounts[i] = 1;
        parentCounts[i] = 0;
        oct.setNext(-1);


        if (task.totalParticles <= 0) continue;

        oct.setNext(this->nodes[task.root].getNext());
        this->nodes[task.root].setFirstChild(root);

        for (int j = 0; j < task.totalParticles; j++) {
            const int particleId = taskParticles[i * maxParticlesPerTask + j];
            this->insert(p->getPositions()[particleId], p->getMasses()[particleId], i);
        }
         
        this->propagate(i);
    }
      
    for(int i = maxNodes-totalTasks; i < maxNodes; i++){
        if(this->tasks[i%totalTasks].totalParticles > 0){
            this->parents[parentCount] = i;
            parentCount++;
        }
    }

    this->propagate();
       
    this->prune();
        
}


void ParallelOctreeCPU::prune() {
    for (int i = 0; i < maxNodes-totalTasks; i++) {
        const int parentIndex = parents[i];
        Node &parent = this->nodes[parentIndex];
        if(!parent.isOccupied()){
            parent.setFirstChild(parent.getNext());
            continue;
        }

        int firstChild = -1;
        int lastChild = -1;


        for (int j = 0; j < 8; j++) {
            const int childIndex = parent.getFirstChild()+ j;
            if (this->nodes[childIndex].isOccupied()) {
                if (firstChild == -1) {
                    firstChild = childIndex;
                    lastChild = childIndex;
                    continue;
                }
                this->nodes[lastChild].setNext(childIndex);
                lastChild = childIndex;
            }

        }
        this->nodes[lastChild].setNext(parent.getNext());
        parent.setFirstChild(firstChild);
    }

    #pragma omp parallel for if (maxNodes > 1000)
    for(int i = maxNodes-totalTasks; i < maxNodes; i++){
        if(this->nodes[i].isOccupied()){
            this->nodes[this->nodes[i].getFirstChild()].setNext(this->nodes[i].getNext());
        }
    }

    #pragma omp parallel for 
    for(int i = 0; i < totalTasks; i++){
        Task &task = this->tasks[i];
        if (task.totalParticles <= 0 || task.root < 0) continue;
        this->prune(i);
        // Reset the task
        task.reset();
    }
}

int ParallelOctreeCPU::getMaxNodes() {
    return this->maxNodes + this->maxNodesPerSubtree * this->totalTasks;
}

void ParallelOctreeCPU::propagate() {
    // Propagate
    #pragma omp parallel for if (maxNodes > 1000)
    for(int i = parentCount -1; i >= maxNodes-totalTasks; i--){
        const int parentIndex = parents[i];
        const int childIndex = this->nodes[parentIndex].getFirstChild();
        if (!this->nodes[childIndex].isOccupied())  continue; // The leaf is empty
        this->nodes[parentIndex].setCenterOfMass(this->nodes[childIndex].getCenterOfMass());
        this->nodes[parentIndex].setMass(glm::vec4(this->nodes[childIndex].getMass(), 0.f, 0.f, 0.f));
    }

    for (int i = maxNodes-totalTasks - 1; i >= 0; i--) {
        // Compute the center of mass and mass of the current parent node
        const int parentIndex = parents[i];
        glm::vec4 centerOfMass(0.f);
        glm::vec4 mass(0.f);

        for (int j = 0; j < 8; ++j) {
            const int childIndex = this->nodes[parentIndex].getFirstChild() + j;
            if (!this->nodes[childIndex].isOccupied())  continue; // The leaf is empty
            centerOfMass += this->nodes[childIndex].getCenterOfMass() * this->nodes[childIndex].getMass();
            mass.x += this->nodes[childIndex].getMass();

        }
        this->nodes[parentIndex].setCenterOfMass(centerOfMass/mass.x);
        this->nodes[parentIndex].setMass(mass);
    }
}

void ParallelOctreeCPU::prune(int subTreeId) {
    const int totalParentNodes = parentCounts[subTreeId];

    for (int i = 0; i < totalParentNodes; i++) {
        const int parentIndex = subTreeParents[i+subTreeId*this->maxNodesPerSubtree];
        Node &parent = this->nodes[parentIndex];
        if(!parent.isOccupied()){
            parent.setFirstChild(parent.getNext());
            continue;
        }
        int firstChild = -1;
        int lastChild = -1;


        for (int j = 0; j < 8; j++) {
            const int childIndex = parent.getFirstChild()+ j;
            if (this->nodes[childIndex].isOccupied()) {
                if (firstChild == -1) {
                    firstChild = childIndex;
                    lastChild = childIndex;
                    continue;
                }
                this->nodes[lastChild].setNext(childIndex);
                lastChild = childIndex;
            }

        }
        this->nodes[lastChild].setNext(parent.getNext());
        parent.setFirstChild(firstChild);
    }
}

void ParallelOctreeCPU::propagate(int subTreeId) {
    const int totalParentNodes = parentCounts[subTreeId];


    for (int i = totalParentNodes - 1; i >= 0; i--) {

        // Compute the center of mass and mass of the current parent node
        int parentIndex = subTreeParents[i+subTreeId*this->maxNodesPerSubtree];
        glm::vec4 centerOfMass(0.f);
        glm::vec4 mass(0.f);

        Node &parent = this->nodes[parentIndex];

        for (int j = 0; j < 8; ++j) {
            const int childIndex = parent.getFirstChild() + j;
            Node &child = this->nodes[childIndex];
            if (!child.isOccupied())  continue; // The leaf is empty
            centerOfMass += child.getCenterOfMass() * child.getMass();
            mass.x += child.getMass();
        }
        parent.setCenterOfMass(centerOfMass/mass.x);
        parent.setMass(mass);
    }
}


/**
 * Resets the tasks
 */
void ParallelOctreeCPU::resetArrays() {
#pragma omp parallel for if (totalTasks > 1000)
    for (int i = 0; i < totalTasks; ++i) {
        tasks[i].reset();    
    }
}

void ParallelOctreeCPU::insert(glm::vec4 pos, glm::vec4 mass, int subTreeId) {
    // Start at the root of the octree
    int i = getSubtree(subTreeId);

    // Keep the root node
    const int root = i; 

    // Traverse the tree
    while(i < i+maxNodesPerSubtree){
        Node &node = this->nodes[i];
   
        // Case 1: The node is an empty leaf
        if(node.isLeaf() && !node.isOccupied()){
            // Proceed to insert the particle
            this->insertParticle(pos, mass, i);
            return;
        }

        // Case 2: The node is an occupied leaf.
        if(node.isLeaf() && node.isOccupied()){
            // Get the current particle's position and mass
            glm::vec4 pos2 = node.getCenterOfMass();
            glm::vec4 mass2 = glm::vec4(node.getMass(), 0.f, 0.f, 0.f);

            // Subdivide the current node
            this->subdivide(i, nodeCounts[subTreeId]+root, subTreeId);

            // Get child indexes
            int childIndex1 = this->getNextNode(pos, i);
            int childIndex2 = this->getNextNode(pos2, i);
            const float dist = glm::length2(pos2-pos);

            // Edge case: Both particles are in the same position or very close
            if (dist < 1e-3) {
                this->insertParticle(pos, glm::vec4(mass2.x + mass.x, 0.f, 0.f, 0.f), childIndex1);  // In this case, insert both particles anyway
                return;
            }
            // If both particles go to the same child node we need to keep subdividing
            while (childIndex1 == childIndex2) {

                // Keep subdividing
                this->subdivide(childIndex1, nodeCounts[subTreeId]+root, subTreeId);


                // Recalculate child indexes after further subdivisions
                childIndex1 = this->getNextNode(pos, childIndex1);
                childIndex2 = this->getNextNode(pos2, childIndex2);
            }

            // Once particles are in different children, insert them
            this->insertParticle(pos, mass, childIndex1);
            this->insertParticle(pos2, mass2, childIndex2);

            return;
        }

        // Case 3: The node is internal
        // Traverse the tree
        i = this->getNextNode(pos, i);
    }
}


void ParallelOctreeCPU::subdivide(int i, int firstChild, int subTreeId) {
    // Set the first child to the current node count
    this->nodes[i].setFirstChild(firstChild);

    // Mark this node as a parent
    this->subTreeParents[subTreeId * this->maxNodesPerSubtree + this->parentCounts[subTreeId]] = i;
    this->parentCounts[subTreeId]++;

    // Create the next child nodes
    this->nodeCounts[subTreeId] += 8;

    const glm::vec4 maxB = this->nodes[i].getMaxBoundary();
    const glm::vec4 minB = this->nodes[i].getMinBoundary();

    const float width = maxB.x - minB.x;
    const float height = maxB.y - minB.y;
    const float depth = maxB.z - minB.z;

    int currentChild = firstChild;

    for (int j = 0; j < 8; ++j) {
        // Create the boundaries for the new node
        glm::vec4 minBoundary(0.f);
        glm::vec4 maxBoundary(0.f);

        // Check the 3rd bit
        if (j & 4) {
            minBoundary.z = minB.z;
            maxBoundary.z = minB.z + (depth/2.f);
        }
        else {
            minBoundary.z = minB.z + (depth/2.f);
            maxBoundary.z = maxB.z;
        }

        // Check the second bit
        if (j & 2) {
            minBoundary.y = minB.y;
            maxBoundary.y = minB.y + (height/2.f);
        }
        else {
            minBoundary.y = minB.y + (height/2.f);
            maxBoundary.y = maxB.y;
        }

        // Check the first bit
        if (j & 1) {
            minBoundary.x = minB.x;
            maxBoundary.x = minB.x + (width/2.f);
        }
        else {
            minBoundary.x = minB.x + (width/2.f);
            maxBoundary.x = maxB.x;
        }


        this->nodes[currentChild].createEmptyNode(minBoundary, maxBoundary);

        // Set the next node
        if (j < 7) {
            // The last child is different
            this->nodes[currentChild].setNext(firstChild+ j + 1);
        }

        // Next child node
        currentChild = firstChild + j + 1;
    }

    // The last child node points to the parent's next
    this->nodes[firstChild+7].setNext(this->nodes[i].getNext());
}

/**
 * @param treeId
 * @return root of the subtree
 */
int ParallelOctreeCPU::getSubtree(int treeId) {
    return this->maxNodes + this->maxNodesPerSubtree * treeId;
}