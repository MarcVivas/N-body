//
// Created by marc on 12/03/23.
//

#include "ParticleSolverCPUSequential.h"


void ParticleSolverCPUSequential::updateParticlePositions(ParticleSystem *particles, float deltaTime){
    for(size_t i =  0; i<particles->size(); i++){
        glm::vec3 newAcceleration = glm::vec3(0,0,0);
        particles->updateParticlePosition(i, deltaTime, newAcceleration);
    }
}

bool ParticleSolverCPUSequential::usesGPU() {return false;}