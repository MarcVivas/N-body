

#ifndef N_BODY_PARTICLESOLVERCPUPARALLEL_H
#define N_BODY_PARTICLESOLVERCPUPARALLEL_H
#include "ParticleSolver.h"


class ParticleSolverCPUParallel: public ParticleSolver  {
public:
    ParticleSolverCPUParallel(float timeStep, float squaredSoftening);
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
protected:
    float timeStep;
    float squaredSoftening;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
    float G;

};


#endif //N_BODY_PARTICLESOLVERCPUPARALLEL_H
