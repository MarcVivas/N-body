

#ifndef PARALLELOCTREEGPU_H
#define PARALLELOCTREEGPU_H

#include <ComputeShader.h>
#include <OpenGLBuffer.h>
#include <ParticleSystem.h>
#include <cstdint>
#include <memory>

struct GPUNode {
    glm::vec3 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2)
    float mass;
    glm::vec3 minBoundary;
    uint32_t numChildren;
    glm::vec3 maxBoundary;
    float padding2;
    int children[8];
};

class ParallelOctreeGPU{
public:
    ParallelOctreeGPU(int n);
    virtual ~ParallelOctreeGPU();
    void insert(ParticleSystem* p);
    int getMaxNodes() const {
        return this->fatherMaxNodes + this->totalParticles * 21;
    }
	int getFatherTreeNodes() const{
		return this->fatherMaxNodes;
	}
    void reset(ParticleSystem* p);

    int totalTasks;
    int maxDepth;
    int fatherMaxNodes;
	int nodeCount;
	int parentCount;
    int totalParticles;






    OpenGLBuffer intermediateBoundsBufferA, intermediateBoundsBufferB, nodesBuffer, tasksBuffer, particlesOffsetsBuffer, blockSumsBuffer, taskParticlesIndexesBuffer, subTreeNodeCountsBuffer, subTreeParentCountsBuffer, assertBuffer;
    OpenGLBuffer gpuNodeBuffer, binaryTreeBuffer, binaryTreeEdges, binaryParentsBuffer, octreeParentsBuffer;
private:
    void initSSBOs(int n);
	void computeBoundingBox(ParticleSystem* p);
    void prune();
    void propagate();
    void resetTasksBuffer();
    void initComputeShaders();
    void expandTree();
    void prepareTasks(ParticleSystem* p);
    void executeTasks();
    std::unique_ptr<Shader> expander, distributor, inserter, resetOctree, boundingBoxPass1, boundingBoxPass2, resetTasks, countParticlesPerTask, prefixSum, addBlockSums, propagateFather, pruneFather, pruneSubtrees;
    std::unique_ptr<Shader> buildBinaryTreeShader, buildOctreeShader, assignParentsAndChildrenShader, propagateOctreeShader;
};



#endif //PARALLELOCTREEGPU_H
