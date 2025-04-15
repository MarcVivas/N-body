
#include "ParticleSolver.h"
#include "Octree.h"

#ifndef N_BODY_PARTICLESOLVERBarnesHut
#define N_BODY_PARTICLESOLVERBarnesHut


class ParticleSolverBHutCPUSeq: public ParticleSolver  {
public:
    ParticleSolverBHutCPUSeq(float timeStep, float squaredSoftening, int n);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
    ~ParticleSolverBHutCPUSeq();

protected:
    Octree *octree;
    float squaredSoftening;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};


#endif //N_BODY_PARTICLESOLVERBarnesHut
