
#include <Shader.h>

#include "ParticleSolverGPU.h"
#include "Octree.h"

#ifndef N_BODY_PARTICLESOLVERBarnesHutParallel_GPUHybridH
#define N_BODY_PARTICLESOLVERBarnesHutParallel_GPUHybridH


class ParticleSolverBHutGPUHybrid: public ParticleSolver  {
public:
    ParticleSolverBHutGPUHybrid(float timeStep, float squaredSoftening, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
    bool usesBH() override;
    Octree* getOctree();
    GLsync gSync;

    ~ParticleSolverBHutGPUHybrid();
protected:
    Octree *octree;
    float squaredSoftening;
    Shader *positionCalculator, *forceCalculator;
    double blockSize;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};


#endif //N_BODY_PARTICLESOLVERBarnesHutParallel_GPUHybridH
