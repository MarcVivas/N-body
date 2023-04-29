
#include "ParticleSystemLagrange.h"

ParticleSystemLagrange::ParticleSystemLagrange() {
    this->totalParticles = 3;
}

ParticleSystem* ParticleSystemLagrange::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    // Set the mass of each particle to be equal
    float mass = 5.f;

    // Calculate the positions of the particles
    float sideLength = glm::length(worldDimensions) / 3.0f;
    glm::vec3 centerPos(worldDimensions.x / 2.0f, worldDimensions.y / 2.0f, worldDimensions.z / 2.0f);
    glm::vec3 vertex1 = centerPos + glm::vec3(0.0f, sideLength / sqrtf(3), 0.0f);
    glm::vec3 vertex2 = centerPos + glm::vec3(-sideLength / 2.0f, -sideLength / (2.0f * sqrtf(3)), 0.0f);
    glm::vec3 vertex3 = centerPos + glm::vec3(sideLength / 2.0f, -sideLength / (2.0f * sqrtf(3)), 0.0f);
    particles[0] = Particle(vertex1, glm::vec3(0), mass);
    particles[1] = Particle(vertex2, glm::vec3(0), mass);
    particles[2] = Particle(vertex3, glm::vec3(0), mass);


    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}


