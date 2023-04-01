//
// Created by marc on 12/03/23.
//

#include "ParticleSolverCPUParallel.h"


void ParticleSolverCPUParallel::updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities,  float deltaTime){
#pragma omp parallel for schedule(static)
    for(size_t i =  0; i<particles.size(); i++){
        glm::vec3 newAcceleration = glm::vec3(0,0,0);
        particles[i].updatePosition(deltaTime, newAcceleration);
        positions[i] = particles[i].position;
        velocities[i] = particles[i].velocity;
    }
}


bool ParticleSolverCPUParallel::usesGPU() {return false;}