#include "ParticleSystemGalaxyInitializer.h"
#include <random>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>

ParticleSystemGalaxyInitializer::ParticleSystemGalaxyInitializer(size_t numParticles) :totalParticles(numParticles){
}


ParticleSystem* ParticleSystemGalaxyInitializer::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    std::random_device randomDevice;
    std::mt19937 mt(randomDevice());
    std::uniform_real_distribution<float> randRadius(0.f, worldDimensions.x / 2.f);
    std::normal_distribution<float> randTheta(0.0f, 2*M_PI);
    float length = glm::length(worldDimensions) / 20.f;

    // Define the parameters for the bulge
    float bulgeRadius = worldDimensions.x / 10.f;
    float bulgeMass = totalParticles * 0.2f;
    float bulgeDensity = bulgeMass / (4.f/3.f * M_PI * pow(bulgeRadius, 3.f));
    float bulgeSoftening = 0.2f;

    // Define the parameters for the spiral arms
    float armRadius = worldDimensions.x / 2.f;
    float armMass = totalParticles * 0.8f;
    float armDensity = armMass / (4.f/3.f * M_PI * pow(armRadius, 3.f));
    float armSoftening = 1.5f;

    glm::vec3 center = glm::vec3(worldDimensions.x / 2.f, worldDimensions.y / 2.f, worldDimensions.z / 2.f);

    for (size_t i = 0; i < totalParticles; i++) {
        float radius = randRadius(mt);
        float theta = randTheta(mt) * 2.f * glm::pi<float>();
        float height = (worldDimensions.z == 0) ? 0 : randRadius(mt) - worldDimensions.x / 2.f;
        glm::vec3 particlePos = center + glm::vec3(radius * std::cos(theta), radius * std::sin(theta), height);

        // Compute the mass based on the particle's position
        float mass = armDensity;
        if (radius < bulgeRadius) {
            mass = bulgeDensity;
        }
        glm::vec3 initialVel = glm::vec3(-length * sin(theta), length * cos(theta), 0.f);
        particles[i] = Particle(particlePos, initialVel, mass);

        // Add an additional velocity component for particles in the spiral arms
        if (radius < armRadius) {
            float armVelocityMagnitude = length * radius / armRadius;
            glm::vec3 armVelocity = glm::vec3(armVelocityMagnitude * -cos(theta), armVelocityMagnitude * -sin(theta), 0.f);
            glm::vec3 newVel = initialVel + armVelocity;
            particles[i].setVelocity(newVel);
        }
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}