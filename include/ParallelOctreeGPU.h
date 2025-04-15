

#ifndef PARALLELOCTREEGPU_H
#define PARALLELOCTREEGPU_H

#include <ComputeShader.h>
#include <OpenGLBuffer.h>
#include <ParticleSystem.h>
#include <memory>



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






    OpenGLBuffer intermediateBoundsBufferA, intermediateBoundsBufferB, parentsBuffer, nodesBuffer, tasksBuffer, particlesOffsetsBuffer, blockSumsBuffer, taskParticlesIndexesBuffer, subTreeParentsBuffer, subTreeNodeCountsBuffer, subTreeParentCountsBuffer, assertBuffer;

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
};



#endif //PARALLELOCTREEGPU_H
