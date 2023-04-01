//
// Created by marc on 30/03/23.
//

#include "ParticleSolverGPU.h"

ParticleSolverGPU::ParticleSolverGPU(std::string &pathToComputeShader): ParticleSolver() {
#ifdef _WIN32
    std::replace(pathToComputeShader.begin(), pathToComputeShader.end(), '/', '\\');
#endif

    this->computeShader = new Shader(pathToComputeShader.c_str());
}

void ParticleSolverGPU::updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities, float deltaTime) {
    this->computeShader->use();
    this->computeShader->setFloat("deltaTime", deltaTime);
    this->computeShader->setInt("numParticles", particles.size());
    glDispatchCompute((int) (particles.size() / 64), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}


ParticleSolverGPU::~ParticleSolverGPU() noexcept {
    delete this->computeShader;
}

bool ParticleSolverGPU::usesGPU() {return true;}