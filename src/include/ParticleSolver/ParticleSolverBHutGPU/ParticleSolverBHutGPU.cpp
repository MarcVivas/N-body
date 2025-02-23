
#include "ParticleSolverBHutGPU.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBHutGPU::ParticleSolverBHutGPU(float stepSize, float squaredSoft, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);

    this->blockSize = 64.0;

    this->positionCalculator = new ComputeShader(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);

    this->forceCalculator = new ComputeShader(forceCalculatorPath);
    this->forceCalculator->use();
    this->forceCalculator->setFloat("squaredSoftening", squaredSoft);
}


void ParticleSolverBHutGPU::updateParticlePositions(ParticleSystem *particles){
    this->octree->reset(particles);

    for(size_t i = 0; i < particles->size(); i++){
        this->octree->insert(particles->getPositions()[i], particles->getMasses()[i]);
    }


    this->octree->propagate();


    this->octree->prune();

    // Dispatch Compute Shader
    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    this->forceCalculator->setFloat("G", this->G);
    this->forceCalculator->setFloat("theta", this->octree->theta);
    this->forceCalculator->setFloat("squaredSoftening", this->squaredSoftening);
    glDispatchCompute(ceil(particles->size() / this->blockSize), 1, 1);

    // Ensure GPU writes are complete before CPU reads
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Insert a fence to synchronize CPU/GPU
    if (gSync) {
        glDeleteSync(gSync);  // Delete the old fence before creating a new one
    }
    gSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

    // Wait for GPU to finish (avoid infinite loop)
    if (gSync) {
        GLenum waitReturn;
        do {
            waitReturn = glClientWaitSync(gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000); // 1-second timeout
        } while (waitReturn == GL_TIMEOUT_EXPIRED);
    }



    // Update particles positions
    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}



bool ParticleSolverBHutGPU::usesBH() {
    return true;
}

Octree* ParticleSolverBHutGPU::getOctree() {
    return octree;
}

void
ParticleSolverBHutGPU::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    particles->getForces()[particleId] = this->octree->computeGravityForce(particles->getPositions()[particleId], this->squaredSoftening, this->G);
}

bool ParticleSolverBHutGPU::usesGPU() {return false;}


float ParticleSolverBHutGPU::getSquaredSoftening() {
    return this->squaredSoftening;
}

ParticleSolverBHutGPU::~ParticleSolverBHutGPU() {
    delete this->octree;
    delete this->positionCalculator;
    delete this->forceCalculator;
}