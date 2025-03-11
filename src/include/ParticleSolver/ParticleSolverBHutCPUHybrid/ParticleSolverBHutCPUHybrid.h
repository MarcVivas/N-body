
#include "ParticleSolver.h"
#include "Octree.h"

#ifndef N_BODY_PARTICLESOLVERBarnesHutParallel_H
#define N_BODY_PARTICLESOLVERBarnesHutParallel_H


class ParticleSolverBHutCPUHybrid: public ParticleSolver  {
public:
    ParticleSolverBHutCPUHybrid(float timeStep, float squaredSoftening, int n);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    bool usesBH() override;
    float getSquaredSoftening() override;
    ~ParticleSolverBHutCPUHybrid();
    Octree* getOctree();
protected:
    Octree *octree;
    float squaredSoftening;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};


#endif //N_BODY_PARTICLESOLVERBarnesHutParallel_H
