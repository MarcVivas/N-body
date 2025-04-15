


#ifndef N_BODY_PARTICLESOLVERBarnesHutParallel_H
#define N_BODY_PARTICLESOLVERBarnesHutParallel_H
#include "ParticleSolver.h"
#include "ParallelOctreeCPU.h"

class ParticleSolverBHutCPUParallel: public ParticleSolver  {
public:
    ParticleSolverBHutCPUParallel(float timeStep, float squaredSoftening, int n);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
    ~ParticleSolverBHutCPUParallel();
protected:
    ParallelOctreeCPU *octree;
    float squaredSoftening;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};


#endif //N_BODY_PARTICLESOLVERBarnesHutHybrid_H
