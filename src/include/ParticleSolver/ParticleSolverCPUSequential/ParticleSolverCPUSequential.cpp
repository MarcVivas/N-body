
#include "ParticleSolverCPUSequential.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>

ParticleSolverCPUSequential::ParticleSolverCPUSequential(float stepSize, float squaredSoft): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
}

void ParticleSolverCPUSequential::updateParticlePositions(ParticleSystem *particles){
    using namespace std::chrono;

    auto start_force = high_resolution_clock::now();
    for(size_t i = 0; i<particles->size(); i++){
        this->computeGravityForce(particles, i);
    }
    auto end_force = high_resolution_clock::now();
    auto duration_force = duration_cast<microseconds>(end_force - start_force);

    auto start_update = high_resolution_clock::now();
    for(size_t i = 0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep);
    }
    auto end_update = high_resolution_clock::now();
    auto duration_update = duration_cast<microseconds>(end_update - start_update);

    std::cout << "--- Time Profiling for updateParticlePositions (Sequential) ---" << std::endl;
    std::cout << "Force Compute Time:    " << duration_force.count() << " microseconds" << std::endl;
    std::cout << "Position Update Time:  " << duration_update.count() << " microseconds" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
}

void
ParticleSolverCPUSequential::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {

    glm::vec4 particlePosition = particles->getPositions()[particleId];

    glm::vec4 totalForce (0.f);

    for(size_t j = 0; j < particles->size(); j++){
        const glm::vec4 vector_i_j = particles->getPositions()[j] - particlePosition;
        const float distance_i_j = glm::length2(vector_i_j) + this->squaredSoftening;
        totalForce += ((G * particles->getMasses()[j].x) / distance_i_j) * vector_i_j;
    }


    particles->getForces()[particleId] = totalForce;

}

bool ParticleSolverCPUSequential::usesGPU() {return false;}


float ParticleSolverCPUSequential::getSquaredSoftening() {
    return this->squaredSoftening;
}