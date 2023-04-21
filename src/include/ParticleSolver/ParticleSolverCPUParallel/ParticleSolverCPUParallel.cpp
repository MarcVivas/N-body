//
// Created by marc on 12/03/23.
//

#include "ParticleSolverCPUParallel.h"

ParticleSolverCPUParallel::ParticleSolverCPUParallel(): ParticleSolver() {
    this->squaredSoftening = 0.5f;
}

void ParticleSolverCPUParallel::updateParticlePositions(ParticleSystem *particles, float deltaTime){
#pragma omp parallel for schedule(static) shared(particles)
    for(size_t i =  0; i<particles->size(); i++){
        glm::vec3 newAcceleration = glm::vec3(0,0,0);
        particles->updateParticlePosition(i, deltaTime, newAcceleration);
    }
}


bool ParticleSolverCPUParallel::usesGPU() {return false;}


float ParticleSolverCPUParallel::getSquaredSoftening() {
    return this->squaredSoftening;
}