
#include "ParticleSolverGPUGrid.h"
#include <glad/glad.h>
#include <iostream>
ParticleSolverGPUGrid::ParticleSolverGPUGrid(GridGPU *gridGpu, float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath): ParticleSolver() {
    this->blockSize = 64.0;

    this->positionCalculator = new ComputeShader(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);


    this->forceCalculator = new ComputeShader(forceCalculatorPath);
    this->grid = gridGpu;
}

ParticleSolverGPUGrid::ParticleSolverGPUGrid(GridGPU *gridGpu, double block_size, float stepSize, float squaredSoft,
                                     std::string &positionCalculatorPath, std::string &forceCalculatorPath) {
    this->blockSize = block_size;

    this->positionCalculator = new ComputeShader(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);

    this->forceCalculator = new ComputeShader(forceCalculatorPath);
    this->grid = gridGpu;

}

void ParticleSolverGPUGrid::updateParticlePositions(ParticleSystem *particles) {
    this->forceCalculator->use();
    glDispatchCompute(ceil(particles->size() / this->blockSize), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


ParticleSolverGPUGrid::~ParticleSolverGPUGrid() noexcept {
    delete this->positionCalculator;
    delete this->forceCalculator;
    delete this->grid;
}

bool ParticleSolverGPUGrid::usesGPU() {return true;}

float ParticleSolverGPUGrid::getSquaredSoftening() {
    return this->squaredSoftening;
}