#include "Grid.h"
#include "ComputeShader.h"

#ifndef N_BODY_GRIDGPU_H
#define N_BODY_GRIDGPU_H


class GridGPU: public Grid{
public:
    GridGPU(std::string shaderPath, glm::vec3 worldDimensions, unsigned int bucketCapacity, unsigned int bucketsPerDimension);
    void updateGrid(ParticleSystem *particleSystem) override;
    ~GridGPU() override;
    void resetGrid() override;
    Bucket* getBucketByPosition(glm::vec4 particlePos) override;
    Bucket* getBucketById(unsigned int bucketId) override;
    unsigned int getTotalBuckets() override;
    Shader *gridShader;
};


#endif //N_BODY_GRIDGPU_H
