//
// Created by marc on 12/03/23.
//

#include "../interface/ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
#define N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H


class ParticleSolverCPUSequential: public ParticleSolver  {
    void updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities,  float deltaTime) override;

};


#endif //N_BODY_PARTICLESOLVERCPUSEQUENTIAL_H
