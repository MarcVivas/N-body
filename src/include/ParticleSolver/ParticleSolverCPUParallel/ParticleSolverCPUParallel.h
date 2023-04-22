
#include "ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUPARALLEL_H
#define N_BODY_PARTICLESOLVERCPUPARALLEL_H


class ParticleSolverCPUParallel: public ParticleSolver  {
public:
    ParticleSolverCPUParallel();
    void updateParticlePositions(ParticleSystem *particles, float deltaTime) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
};


#endif //N_BODY_PARTICLESOLVERCPUPARALLEL_H
