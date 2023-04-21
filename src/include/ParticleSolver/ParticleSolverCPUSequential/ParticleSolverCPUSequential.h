//
// Created by marc on 12/03/23.
//

#include "../interface/ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
#define N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H


class ParticleSolverCPUSequential: public ParticleSolver  {
public:
    ParticleSolverCPUSequential();
    void updateParticlePositions(ParticleSystem *particles, float deltaTime) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
};


#endif //N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
