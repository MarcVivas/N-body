
#include "ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
#define N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H


class ParticleSolverCPUSequential: public ParticleSolver  {
public:
    ParticleSolverCPUSequential(float timeStep, float squaredSoftening);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
    float G;
    float timeStep;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
};


#endif //N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
