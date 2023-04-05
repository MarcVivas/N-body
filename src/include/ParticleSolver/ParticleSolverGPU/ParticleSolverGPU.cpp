//
// Created by marc on 30/03/23.
//

#include "ParticleSolverGPU.h"

ParticleSolverGPU::ParticleSolverGPU(std::string &pathToComputeShader): ParticleSolver() {
    this->computeShader = new ComputeShader(pathToComputeShader);
}

void ParticleSolverGPU::updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities, float deltaTime) {
    this->computeShader->use();
    this->computeShader->setFloat("deltaTime", deltaTime);
    this->computeShader->setInt("numParticles", particles.size());
    glDispatchCompute(ceil(particles.size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


ParticleSolverGPU::~ParticleSolverGPU() noexcept {
    delete this->computeShader;
}

bool ParticleSolverGPU::usesGPU() {return true;}