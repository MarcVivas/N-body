//
// Created by marc on 12/03/23.
//

#include "ParticleSolverCPUParallel.h"


void ParticleSolverCPUParallel::updateParticlePositions(std::vector<Particle> &particles, float deltaTime){
#pragma omp parallel for schedule(static)
    for(size_t i =  0; i<particles.size(); i++){
        glm::vec3 newAcceleration = glm::vec3(0,0,0);
        particles[i].updatePosition(deltaTime, newAcceleration);
    }
}