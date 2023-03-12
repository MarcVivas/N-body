//
// Created by marc on 12/03/23.
//

#include "../interface/ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
#define N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H


class ParticleSolverCPUSequential: public ParticleSolver  {
    void updateParticlePositions(std::vector<Particle> &particles, float deltaTime) override;

};


#endif //N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
