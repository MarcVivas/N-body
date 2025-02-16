
#include "ParticleSolverCPUGrid.h"
#include <glm/gtx/norm.hpp>
#include <iostream>
ParticleSolverCPUGrid::ParticleSolverCPUGrid(GridCPU *grid, float stepSize, float squaredSoft): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->gridCpu = grid;
}

void ParticleSolverCPUGrid::updateParticlePositions(ParticleSystem *particles){

    this->gridCpu->updateGrid(particles);

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
ParticleSolverCPUGrid::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    glm::vec4 particlePosition = particles->getPositions()[particleId];
    glm::vec4 totalForce (0.f);

    // Get the bucket where the particle is located
    Bucket* bucket = this->gridCpu->getBucketByPosition(particlePosition);

    // Compute forces inside the bucket
    for(size_t j = 0; j < bucket->getNumParticles(); j++){
        const unsigned int otherParticleId = bucket->getParticleId(j);
        const glm::vec4 vector_i_j = particles->getPositions()[otherParticleId] - particlePosition;
        const float distance_i_j = glm::length2(vector_i_j) + this->squaredSoftening;
        totalForce += ((G * particles->getMasses()[otherParticleId].x) / distance_i_j) * vector_i_j;
    }

    // Compute the forces with other buckets

    Bucket *otherBucket = nullptr;
    for(size_t bucketId = 0; bucketId < this->gridCpu->getTotalBuckets(); bucketId++){
        otherBucket = this->gridCpu->getBucketById(bucketId);
        if (bucket->getBucketId() != otherBucket->getBucketId()){
            const glm::vec4 centerOfMass = otherBucket->getCenterOfMass();
            const float mass = centerOfMass.w;
            const glm::vec4 centerOfMassPosition = glm::vec4(centerOfMass.x, centerOfMass.y, centerOfMass.z, 0.f);
            const glm::vec4 vector_i_j = centerOfMassPosition - particlePosition;
            const float distance_i_j = std::pow(glm::length2(vector_i_j) + this->squaredSoftening, 1.5);
            totalForce += ((G * mass) / distance_i_j) * vector_i_j;
        }
    }

    particles->getForces()[particleId] = totalForce;
}

bool ParticleSolverCPUGrid::usesGPU() {return false;}

ParticleSolverCPUGrid::~ParticleSolverCPUGrid() noexcept {
    delete this->gridCpu;
}

float ParticleSolverCPUGrid::getSquaredSoftening() {
    return this->squaredSoftening;
}