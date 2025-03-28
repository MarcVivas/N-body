#ifndef N_BODY_PARTICLESOLVERBHUTGPU_H
#define N_BODY_PARTICLESOLVERBHUTGPU_H

#include <ParallelOctreeCPU.h>
#include <ParallelOctreeGPU.h>
#include <Shader.h>

#include "ParticleSolverGPU.h"
#include "Octree.h"


class ParticleSolverBHutGPU: public ParticleSolver  {
public:
    ParticleSolverBHutGPU(float timeStep, float squaredSoftening, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
    bool usesBH() override;
    Octree* getOctree();
    GLsync gSync;

    ~ParticleSolverBHutGPU();
protected:
    ParallelOctreeCPU *octree;
    float squaredSoftening;
    Shader *positionCalculator, *forceCalculator;
    double blockSize;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};

#endif //N_BODY_PARTICLESOLVERBHUTGPU_H