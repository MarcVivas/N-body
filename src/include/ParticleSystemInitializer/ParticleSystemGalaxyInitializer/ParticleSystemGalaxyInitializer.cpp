#include "ParticleSystemGalaxyInitializer.h"
#include <random>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

ParticleSystemGalaxyInitializer::ParticleSystemGalaxyInitializer(size_t numParticles) :totalParticles(numParticles){
}


ParticleSystem* ParticleSystemGalaxyInitializer::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> randRadius(0.f, worldDimensions.x);
    std::uniform_real_distribution<float> randThickness(0.1f, worldDimensions.z / 5.f);
    std::normal_distribution<float> randTheta(0.0f, M_PI * 50);
    float length = glm::length(worldDimensions) / 4.f;

    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 0.5;

    // Define the parameters for the bulge
    float bulgeRadius = worldDimensions.x / 10.f;

    // Define the parameters for the spiral arms
    float armRadius = worldDimensions.x / 2.f;

    glm::vec3 center = glm::vec3(worldDimensions.x / 2.f, worldDimensions.y / 2.f, worldDimensions.z / 2.f);

    for (size_t i = 0; i < totalParticles; i++) {
        float radius = randRadius(mt);
        float theta = randTheta(mt) * 2.f * glm::pi<float>();
        float height = (worldDimensions.z == 0) ? 0 : randThickness(mt);
        glm::vec3 particlePos = center + glm::vec3(radius * std::cos(theta), radius * std::sin(theta), height);

        // Compute the mass based on the particle's position
        if (radius < bulgeRadius) {
            particleMass = (worldVolume / this->totalParticles) * 0.7;
        } else {
            particleMass = (worldVolume / this->totalParticles) * 0.5;
        }

        glm::vec3 initialVel = glm::vec3(-length * sin(theta), length * cos(theta), 0.f);

        particles[i] = Particle(particlePos, initialVel, particleMass);
        if (radius < armRadius) {
            // Calculate the angle between the particle's position vector and the radial vector
            float angle = std::atan2(particlePos.y - center.y, particlePos.x - center.x);
            float armVelocityMagnitude = length * (radius / armRadius) * std::cos(6.f * (angle - theta));
            glm::vec3 armVelocity = glm::vec3(armVelocityMagnitude * -cos(theta), armVelocityMagnitude * -sin(theta), 0.f);
            glm::vec3 newVel = initialVel + armVelocity;
            particles[i].setVelocity(newVel);
        }
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}