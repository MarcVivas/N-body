#include <iostream>
#include <omp.h>

#include "Octree.h"

#ifndef PARALLELOCTREECPU_H
#define PARALLELOCTREECPU_H


struct Task {
    int root;
    int totalParticles;
    // Default Constructor
    Task() : root(-1), totalParticles(0){}
  
    void reset() {
        totalParticles = 0;
    }
};

class ParallelOctreeCPU: public Octree {
public:
    ParallelOctreeCPU(int n);
    virtual ~ParallelOctreeCPU();
    void insert(ParticleSystem *p);
    int getMaxNodes() override;
    void reset(ParticleSystem *p) override;
    Task *tasks;
    int totalTasks;

    int maxDepth;
    int numThreads;
    int maxNodesPerSubtree;
    int maxParticlesPerTask;

    int *nodeCounts;
    int *subTreeParents;
    int *parentCounts;
    int *taskParticles;

    std::vector<omp_lock_t> nodeLocks;

    private:
        void prune() override;
        void propagate() override;
        int getSubtree(int treeId);
        void subdivide(int i, int firstChild, int root);
        void resetArrays();
        void countParticlesPerNode(int particleCount);
        void copyActiveParticles(int particleCount);
        void assignParticlesToNodes(int particleCount, ParticleSystem *p);
        void classifyNodes(int firstNode);
        void subdivideNodes(int &firstNode);
        void prepareParticlesForNextIteration(int &particleCount);
        void processParticlesForNextIteration(int particleCount, ParticleSystem *p);
        void insert(glm::vec4 pos, glm::vec4 mass, int root) override;
        void propagate(int subTree);
        void prune(int subTree);
};



#endif //PARALLELOCTREECPU_H
