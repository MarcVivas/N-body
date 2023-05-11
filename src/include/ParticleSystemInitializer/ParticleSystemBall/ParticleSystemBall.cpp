#include "ParticleSystemBall.h"
#include <glm/gtc/random.hpp>
#include <iostream>

ParticleSystemBall::ParticleSystemBall(size_t numParticles) : totalParticles(numParticles){}

ParticleSystem* ParticleSystemBall::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    float length = glm::length(worldDimensions) / 20.f;


    // Determine radius of sphere based on world dimensions
    float radius = glm::length(worldDimensions) / 3.0f;

    // Determine particle mass based on world volume and total particles
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 1.0;

    for (int i = 0; i < totalParticles; ++i) {
        glm::vec3 position = glm::ballRand(radius);
        // Shift particle position by half of the world dimensions
        position += 0.5f * worldDimensions;
        glm::vec3 initialVel = (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
        particles[i] = Particle(position, initialVel, particleMass);
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}