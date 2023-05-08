
#include "ParticleSolverGPU.h"
#include <glad/glad.h>

ParticleSolverGPU::ParticleSolverGPU(float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath): ParticleSolver() {
    this->blockSize = 64.0;

    this->positionCalculator = new ComputeShader(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);

    this->forceCalculator = new ComputeShader(forceCalculatorPath);
    this->forceCalculator->use();
    this->forceCalculator->setFloat("squaredSoftening", squaredSoft);
}

void ParticleSolverGPU::updateParticlePositions(ParticleSystem *particles) {
    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / this->blockSize), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / this->blockSize), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


ParticleSolverGPU::~ParticleSolverGPU() noexcept {
    delete this->positionCalculator;
    delete this->forceCalculator;
}

bool ParticleSolverGPU::usesGPU() {return true;}

float ParticleSolverGPU::getSquaredSoftening() {
    return this->squaredSoftening;
}