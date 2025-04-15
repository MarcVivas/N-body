
#include "ParticleSolverCPUParallel.h"
#include <omp.h>
#include <glm/gtx/norm.hpp>
#include <iostream>
ParticleSolverCPUParallel::ParticleSolverCPUParallel(float stepSize, float squaredSoft): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
}

void ParticleSolverCPUParallel::updateParticlePositions(ParticleSystem *particles){

    #pragma omp parallel for schedule(static) shared(particles)
    for(size_t i =  0; i < particles->size(); i++){
        this->computeGravityForce(particles, i);
    }

    #pragma omp parallel for schedule(static) shared(particles)
    for(size_t i =  0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep);
    }
}


void
ParticleSolverCPUParallel::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    glm::vec4 particlePosition = particles->getPositions()[particleId];
    glm::vec4 totalForce (0.f);

    for(size_t j = 0; j < particles->size(); j++){
        if (particleId != j) {
            const glm::vec4 vector_i_j = particles->getPositions()[j] - particlePosition;
            const float distance_i_j = std::pow(glm::length2(vector_i_j) + this->squaredSoftening, 1.5f);
            totalForce += ((G * particles->getMasses()[j].x) / distance_i_j) * vector_i_j;
        }
    }

    particles->getForces()[particleId] = totalForce;
}

bool ParticleSolverCPUParallel::usesGPU() {return false;}


float ParticleSolverCPUParallel::getSquaredSoftening() {
    return this->squaredSoftening;
}