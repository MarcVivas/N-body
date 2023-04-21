//
// Created by marc on 30/03/23.
//

#include "ParticleSolverGPU.h"

ParticleSolverGPU::ParticleSolverGPU(std::string &pathToComputeShader): ParticleSolver() {
    this->computeShader = new ComputeShader(pathToComputeShader);
    this->squaredSoftening = 0.5f;
}

void ParticleSolverGPU::updateParticlePositions(ParticleSystem *particles, float deltaTime) {
    this->computeShader->use();
    this->computeShader->setFloat("deltaTime", deltaTime);
    this->computeShader->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


ParticleSolverGPU::~ParticleSolverGPU() noexcept {
    delete this->computeShader;
}

bool ParticleSolverGPU::usesGPU() {return true;}

float ParticleSolverGPU::getSquaredSoftening() {
    return this->squaredSoftening;
}