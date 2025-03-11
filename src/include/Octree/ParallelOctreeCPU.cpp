
#include "ParallelOctreeCPU.h"
#include <numeric>
#include <execution>
#include <omp.h>
#include <iostream>
#include <glm/gtx/norm.hpp>


ParallelOctreeCPU::ParallelOctreeCPU(const int n): Octree(n) {
    this->numThreads = omp_get_max_threads();
    this->maxDepth = 2;


    this->maxNodes = 0;
    for (int i = 0; i <= maxDepth; i++) {
        maxNodes += std::pow(8, i);
    }


    this->nodeLocks.resize(maxNodes);
    for (int i = 0; i < maxNodes; ++i) {
        omp_init_lock(&nodeLocks[i]);
    }
    this->totalTasks = std::pow(8, maxDepth);
    this->octrees = new Octree*[totalTasks];
    this->tasks = new Task[totalTasks];

    #pragma omp parallel for
    for (int i = 0; i < totalTasks; i++) {
        this->tasks[i] = Task(n/8);
        this->octrees[i] = new Octree(n/2);
    }

    this->resetArrays(n, maxNodes);
}

ParallelOctreeCPU::~ParallelOctreeCPU() {
    delete[] tasks;

    for (int i = 0; i < maxNodes; ++i) {
        omp_destroy_lock(&nodeLocks[i]);
    }

    for (int i = 0; i < totalTasks; i++) {
        delete octrees[i];
    }
    delete[] octrees;

}
#include <chrono>
#include <iostream>
/**
 * Inserts the particles level by level
 * @param p particles
 */
void ParallelOctreeCPU::insert(ParticleSystem *p) {
    int particleCount = p->size();

    // Reset arrays
    this->resetArrays(p->size(), nodeCount);

    int root = 0;
    // Subdivide the tree
    while (nodeCount < maxNodes) {
        this->subdivide(root, this->nodeCount);
        this->nodeCount+=8;
        root+=1;
    }

    // Fill the tasks
    #pragma omp parallel for
    for (int j = 0; j < particleCount; j++) {
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
        this->tasks[i%totalTasks].insert(i, j);
        omp_unset_lock(&nodeLocks[i]);  // Unlock after insertion
    }

    // Distribute work among threads
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < totalTasks; ++i) {
        // Execute task
        Task &task = this->tasks[i];
        Octree *oct = this->octrees[i];

        // Reset octrees
        if (task.totalParticles > 0) {
            oct->getNodes()[0].setBoundingBox(this->nodes[task.root].getMinBoundary(), this->nodes[task.root].getMaxBoundary());
        }
        oct->getNodes()[0].setFirstChild(-1);
        oct->getNodes()[0].setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f));
        oct->nodeCount = 1;
        oct->parentCount = 0;
        oct->getNodes()[0].setNext(-1);

        if (task.totalParticles <= 0) continue;

        for (int j = 0; j < task.totalParticles; j++) {
            const int particleId = task.particles[j];
            oct->insert(p->getPositions()[particleId], p->getMasses()[particleId]);
        }

        oct->propagate();
        oct->prune();
    }

    // Propagate
    for (int i = parentCount - 1; i >= 0; i--) {
        // Compute the center of mass and mass of the current parent node
        const int parentIndex = parents[i];
        glm::vec4 centerOfMass(0.f);
        glm::vec4 mass(0.f);

        #pragma unroll
        for (int j = 0; j < 8; ++j) {
            const int childIndex = this->nodes[parentIndex].getFirstChild() + j;

            if(childIndex >= parentCount){
                Octree *child = this->octrees[childIndex%totalTasks];
                if(!child->getNodes()[0].isOccupied()) continue;
                centerOfMass += child->getNodes()[0].getCenterOfMass() * child->getNodes()[0].getMass();
                mass.x += child->getNodes()[0].getMass();
                this->nodes[childIndex].setMass(glm::vec4(child->getNodes()[0].getMass(), 0.f, 0.f, 0.f));
                this->nodes[childIndex].setCenterOfMass(child->getNodes()[0].getCenterOfMass());
                continue;
            }
            if (!this->nodes[childIndex].isOccupied())  continue; // The leaf is empty
            centerOfMass += this->nodes[childIndex].getCenterOfMass() * this->nodes[childIndex].getMass();
            mass.x += this->nodes[childIndex].getMass();

        }

        this->nodes[parentIndex].setCenterOfMass(centerOfMass/mass.x);
        this->nodes[parentIndex].setMass(mass);
    }

    //this->prune();

}

glm::vec4 ParallelOctreeCPU::computeGravityForce(glm::vec4 &pos, float squaredSoftening, float G) {
    glm::vec4 force(0.f);

    int i = 0; // Root of the tree

    while (i >= 0){
        // We need to compute the size and the distance between the particle and the node
        glm::vec4 size = this->nodes[i].getMaxBoundary() - this->nodes[i].getMinBoundary();
        float s = glm::max(glm::max(size.x, size.y), size.z);
        float s_squared = s*s;


        glm::vec4 centerOfMass = this->nodes[i].getCenterOfMass();
        const glm::vec4 vector_i_j = centerOfMass - pos;
        const float dist_squared = glm::length2(vector_i_j);  // Squared distance

        // If the node satisfies the criteria
        if (s_squared < this->theta * dist_squared) {
            // Compute the force
            if (dist_squared > 0 && this->nodes[i].isOccupied()) {
                const float effective_dist_squared = dist_squared + squaredSoftening; // Apply softening to avoid 0 division
                const float inv_dist = 1.0f / std::pow(effective_dist_squared, 1.5f);
                force += ((vector_i_j * (G * this->nodes[i].getMass())) * inv_dist);
            }

            // Go to the next sibling or parent
            i = this->nodes[i].getNext();
        }
        else if(i >= parentCount){
            if (dist_squared > 0) {
                Octree *oct = this->octrees[i % totalTasks];
                force += oct->computeGravityForce(pos, squaredSoftening, G);
            }

            i = this->nodes[i].getNext();
        }
        else {
            // Go down the tree
            i = this->nodes[i].getFirstChild();
        }
    }
    return force;
}





/**
 * Resets the arrays for the next iteration (or level of the tree)
 */
void ParallelOctreeCPU::resetArrays(int totalParticles, int totalNodes) {
#pragma omp parallel for
    for (int i = 0; i < totalTasks; ++i) {
        if (i < totalTasks) {
            tasks[i].reset();
        }
    }
}

void ParallelOctreeCPU::subdivide(int i, int firstChild) {
    // Set the first child to the current node count
    this->nodes[i].setFirstChild(firstChild);

    // Mark this node as a parent
    this->parents[this->parentCount] = i;
    this->parentCount+=1;

    // Create the next child nodes

    const glm::vec4 maxB = this->nodes[i].getMaxBoundary();
    const glm::vec4 minB = this->nodes[i].getMinBoundary();

    const float width = maxB.x - minB.x;
    const float height = maxB.y - minB.y;
    const float depth = maxB.z - minB.z;

    int currentChild = firstChild;

#pragma unroll
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

