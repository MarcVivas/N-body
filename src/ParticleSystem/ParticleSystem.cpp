#include "ParticleSystem.h"
#include "Helpers.h"
#include <algorithm>
using helpers::log;
ParticleSystem::ParticleSystem(const std::vector<Particle> &particles) {
    this->numParticles = particles.size();
    this->velocities = new glm::vec4[this->numParticles]();
    this->accelerations = new glm::vec4[this->numParticles]();
    this->positions = new glm::vec4[this->numParticles]();
    this->masses = new glm::vec4[this->numParticles]();
    this->forces = new glm::vec4[this->numParticles]();


    for (int i = 0; i < this->numParticles; i++) {
        this->velocities[i] = particles[i].velocity;
        this->accelerations[i] = particles[i].acceleration;
        this->positions[i] = particles[i].position;
        this->masses[i] = glm::vec4(particles[i].mass, 0, 0, 0);
        this->forces[i] = glm::vec4(0.f);
    }

	mortonBuffer.createBufferData(sizeof(glm::uvec2) * this->numParticles, nullptr, 16, GL_DYNAMIC_DRAW);
	mortonShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/morton.glsl"));
    rearrangeParticlesShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/rearrange.glsl"));
}

ParticleSystem::ParticleSystem(ParticleSystem * other) {
    this->numParticles = other->size();
    this->velocities = new glm::vec4[this->numParticles]();
    this->accelerations = new glm::vec4[this->numParticles]();
    this->positions = new glm::vec4[this->numParticles]();
    this->masses = new glm::vec4[this->numParticles]();
    this->forces = new glm::vec4[this->numParticles]();

    for (int i = 0; i < this->numParticles; i++) {
        this->velocities[i] = other->getVelocities()[i];
        this->accelerations[i] = other->getAccelerations()[i];
        this->positions[i] = other->positions[i];
        this->masses[i] = other->masses[i];
        this->forces[i] = other->forces[i];
    }
}

// Sort the particles in the GPU using the z-order curve
// Why? Improve cache locality, memory access patterns and reduce warp divergence
void ParticleSystem::gpuSort() {
	int workGroups = (this->numParticles + 256 - 1) / 256;
	
    mortonShader->use();
	mortonShader->setInt("numParticles", this->numParticles);
	glDispatchCompute(workGroups, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	auto mortonCodes = mortonBuffer.getDataVector<glm::uvec2>(0, sizeof(glm::uvec2) * this->numParticles);
	
    std::sort(mortonCodes.begin(), mortonCodes.end(), [](glm::uvec2 &v1, glm::uvec2 &v2){
        return v1.x < v2.x;
    });

    mortonBuffer.destroy();
	mortonBuffer.createBufferData(sizeof(glm::uvec2) * this->numParticles, mortonCodes.data(), 16, GL_DYNAMIC_DRAW);

    for (int i = 0; i < this->numParticles; i++) {
		log(mortonCodes[i].x, mortonCodes[i].y);
		log(this->positions[i].x, this->positions[i].y, this->positions[i].z);

    }
}








void ParticleSystem::setAccelerations(glm::vec4 *newAccelerations) {
    delete [] this->accelerations;
    this->accelerations = newAccelerations;
}

void ParticleSystem::setMasses(glm::vec4 *newMasses) {
    delete [] this->masses;
    this->masses = newMasses;
}

void ParticleSystem::setForces(glm::vec4 *newForces) {
    delete [] this->forces;
    this->forces = newForces;
}

void ParticleSystem::setPositions(glm::vec4 *newPositions) {
    delete [] this->positions;
    this->positions = newPositions;
}

void ParticleSystem::setVelocities(glm::vec4 *newVelocities) {
    delete [] this->velocities;
    this->velocities = newVelocities;
}

std::ostream& operator<<(std::ostream& os, const ParticleSystem& system) {
    os << "Particle System with " << system.numParticles << " particles:" << std::endl;
    for (unsigned int i = 0; i < system.numParticles; ++i) {
        os << "Particle ID: " << i << std::endl;
        os << "Position: (" << system.positions[i].x << ", " << system.positions[i].y << ", " << system.positions[i].z << ")" << std::endl;
        os << "Velocity: (" << system.velocities[i].x << ", " << system.velocities[i].y << ", " << system.velocities[i].z << ")" << std::endl;
        os << "Acceleration: (" << system.accelerations[i].x << ", " << system.accelerations[i].y << ", " << system.accelerations[i].z << ")" << std::endl;
        os << "Mass: " << system.masses[i].x << std::endl;
    }
    return os;
}