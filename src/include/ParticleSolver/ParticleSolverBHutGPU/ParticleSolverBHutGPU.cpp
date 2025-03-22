
#include "ParticleSolverBHutGPU.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>
#include <ParallelOctreeCPU.h>

ParticleSolverBHutGPU::ParticleSolverBHutGPU(float stepSize, float squaredSoft, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new ParallelOctreeCPU(n);

    this->blockSize = 64.0;

    this->positionCalculator = new ComputeShader(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);

    this->forceCalculator = new ComputeShader(forceCalculatorPath);
    this->forceCalculator->use();
    this->forceCalculator->setFloat("squaredSoftening", squaredSoft);
}

void ParticleSolverBHutGPU::updateParticlePositions(ParticleSystem *particles){
    auto start_reset = std::chrono::high_resolution_clock::now();
    this->octree->reset(particles);
    auto end_reset = std::chrono::high_resolution_clock::now();
    auto duration_reset = std::chrono::duration_cast<std::chrono::microseconds>(end_reset - start_reset);
    std::cout << "Time for reset: " << duration_reset.count() << " microseconds" << std::endl;

    auto start_insert = std::chrono::high_resolution_clock::now();
    this->octree->insert(particles);
    auto end_insert = std::chrono::high_resolution_clock::now();
    auto duration_insert = std::chrono::duration_cast<std::chrono::microseconds>(end_insert - start_insert);
    std::cout << "Time for insert: " << duration_insert.count() << " microseconds" << std::endl;




    // Dispatch Compute Shader
    auto start_force = std::chrono::high_resolution_clock::now();
    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    this->forceCalculator->setFloat("G", this->G);
    this->forceCalculator->setFloat("theta", this->octree->theta);
    this->forceCalculator->setFloat("squaredSoftening", this->squaredSoftening);
    glDispatchCompute(ceil(particles->size() / this->blockSize), 1, 1);
    auto end_force = std::chrono::high_resolution_clock::now();
    auto duration_force = std::chrono::duration_cast<std::chrono::microseconds>(end_force - start_force);
    std::cout << "Time for force: " << duration_force.count() << " microseconds" << std::endl;  

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



bool ParticleSolverBHutGPU  ::usesBH() {
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

ParticleSolverBHutGPU::~ParticleSolverBHutGPU () {
    delete this->octree;
    delete this->positionCalculator;
    delete this->forceCalculator;
}