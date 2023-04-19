//
// Created by marc on 12/03/23.
//

#include "../interface/ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUPARALLEL_H
#define N_BODY_PARTICLESOLVERCPUPARALLEL_H


class ParticleSolverCPUParallel: public ParticleSolver  {
    void updateParticlePositions(ParticleSystem *particles, float deltaTime) override;
    bool usesGPU() override;

};


#endif //N_BODY_PARTICLESOLVERCPUPARALLEL_H
