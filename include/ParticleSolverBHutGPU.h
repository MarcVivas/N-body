#ifndef N_BODY_PARTICLESOLVERBHUTGPU_H
#define N_BODY_PARTICLESOLVERBHUTGPU_H

#include <ParallelOctreeGPU.h>
#include <ComputeShader.h>
#include "Octree.h"
#include "ParticleSolver.h"

class ParticleSolverBHutGPU: public ParticleSolver  {
public:
    ParticleSolverBHutGPU(float timeStep, float squaredSoftening, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override {
        return this->squaredSoftening;
    }
    ~ParticleSolverBHutGPU();
protected:
    const float theta = 0.5 * 0.5;
    std::unique_ptr<ParallelOctreeGPU> octree;
    float squaredSoftening;
    std::unique_ptr<Shader> positionCalculator, forceCalculator;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};

#endif //N_BODY_PARTICLESOLVERBHUTGPU_H