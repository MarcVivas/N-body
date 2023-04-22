#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(std::vector<Particle> &particles) {
    this->numParticles = particles.size();
    this->velocities = new glm::vec4[this->numParticles]();
    this->accelerations = new glm::vec4[this->numParticles]();
    this->positions = new glm::vec4[this->numParticles]();
    this->masses = new glm::vec4[this->numParticles]();

    for (int i = 0; i < this->numParticles; i++) {
        this->velocities[i] = particles[i].velocity;
        this->accelerations[i] = particles[i].acceleration;
        this->positions[i] = particles[i].position;
        this->masses[i] = glm::vec4(particles[i].mass, 0, 0, 0);
    }
}



/**
 * Updates a particle position
 * Performs the leapfrog integration
 * @param particleId
 * @param deltaTime
 * @param newAcceleration
 */
void ParticleSystem::updateParticlePosition(unsigned int particleId, float deltaTime, glm::vec3 &newAcceleration) {
    float dtDividedBy2 = deltaTime/2.f;

    // Compute velocity (i + 1/2)
    this->velocities[particleId] += this->accelerations[particleId] * dtDividedBy2;

    // Compute next position (i+1)
    this->positions[particleId] += this->velocities[particleId] * deltaTime;

    // Update acceleration (i+1)
    this->accelerations[particleId] = glm::vec4(newAcceleration, 0);

    // Compute next velocity (i+1)
    this->velocities[particleId] += this->accelerations[particleId] * dtDividedBy2;
}


unsigned int ParticleSystem::size() {
    return this->numParticles;
}

glm::vec4* ParticleSystem::getMasses(){
    return this->masses;
}

glm::vec4* ParticleSystem::getPositions() {
    return this->positions;
}

glm::vec4* ParticleSystem::getVelocities() {
    return this->velocities;
}

glm::vec4* ParticleSystem::getAccelerations() {
    return this->accelerations;
}

void ParticleSystem::setAccelerations(glm::vec4 *newAccelerations) {
    this->accelerations = newAccelerations;
}

void ParticleSystem::setMasses(glm::vec4 *newMasses) {
    this->masses = newMasses;
}

void ParticleSystem::setPositions(glm::vec4 *newPositions) {
    this->positions = newPositions;
}

void ParticleSystem::setVelocities(glm::vec4 *newVelocities) {
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