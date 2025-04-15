#include "ParticleSystemSphere.h"
#include <glm/gtc/random.hpp>
#include <iostream>

ParticleSystemSphere::ParticleSystemSphere(size_t numParticles) : totalParticles(numParticles){}

std::unique_ptr<ParticleSystem> ParticleSystemSphere::generateParticles(glm::vec3 worldDimensions) {
    std::vector<Particle> particles;
    particles.reserve(this->totalParticles);

    float length = glm::length(worldDimensions) / 20.f;


    // Determine radius of sphere based on world dimensions
    float radius = glm::length(worldDimensions) / 3.0f;

    // Determine particle mass based on world volume and total particles
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 1.5;

    for (int i = 0; i < totalParticles; ++i) {
        glm::vec3 position = glm::sphericalRand(radius);
        // Shift particle position by half of the world dimensions
        position += 0.5f * worldDimensions;
        glm::vec3 initialVel = (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
        particles.push_back(Particle(position, initialVel, particleMass));
    }

    return std::make_unique<ParticleSystem>(particles);
}