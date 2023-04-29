
#include "ParticleSolverCPUParallel.h"
#include <omp.h>
#include <glm/gtx/norm.hpp>

ParticleSolverCPUParallel::ParticleSolverCPUParallel(float stepSize, float squaredSoft): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
}

void ParticleSolverCPUParallel::updateParticlePositions(ParticleSystem *particles){
#pragma omp parallel for schedule(static) shared(particles)
    for(size_t i =  0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep, this->computeGravityAcceleration(particles, i));
    }
}

glm::vec4
ParticleSolverCPUParallel::computeGravityAcceleration(ParticleSystem *particles, const unsigned int particleId) {
    glm::vec4 particlePosition = particles->getPositions()[particleId];
    float particleMass = particles->getMasses()[particleId].x;

    glm::vec4 totalForce (0.f);

    for(size_t j = 0; j < particles->size(); j++){
        const glm::vec4 vector_i_j = particles->getPositions()[j] - particlePosition;
        const float distance_i_j = std::pow(glm::length2(vector_i_j) + this->squaredSoftening, 1.5);
        totalForce += ((G * particleMass * particles->getMasses()[j].x) / distance_i_j) * vector_i_j;
    }

    return totalForce;
}

bool ParticleSolverCPUParallel::usesGPU() {return false;}


float ParticleSolverCPUParallel::getSquaredSoftening() {
    return this->squaredSoftening;
}