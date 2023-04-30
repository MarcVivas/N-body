
#include "ParticleSystemLagrange.h"

ParticleSystemLagrange::ParticleSystemLagrange() {
    this->totalParticles = 3;
}

ParticleSystem* ParticleSystemLagrange::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    // Set the mass of each particle to be equal
    float mass = 10.5f;

    // Calculate the positions of the particles
    float sideLength = glm::length(worldDimensions) / 10.0f;
    glm::vec3 centerPos(worldDimensions.x / 2.0f, worldDimensions.y / 2.0f, worldDimensions.z / 2.0f);
    glm::vec3 vertex1 = centerPos + glm::vec3(0.0f, sideLength / sqrtf(3), 0.0f);
    glm::vec3 vertex2 = centerPos + glm::vec3(-sideLength / 2.0f, -sideLength / (2.0f * sqrtf(3)), 0.0f);
    glm::vec3 vertex3 = centerPos + glm::vec3(sideLength / 2.0f, -sideLength / (2.0f * sqrtf(3)), 0.0f);
    particles[0] = Particle(vertex1, glm::vec3(0), mass);
    particles[1] = Particle(vertex2, glm::vec3(0), mass);
    particles[2] = Particle(vertex3, glm::vec3(0), mass);

    // Compute the center of mass
    glm::vec3 centerOfMass = (vertex1 + vertex2 + vertex3) / 3.0f;

    // Compute the total mass
    float totalMass = mass * this->totalParticles;

    // Compute the distances from the center of mass
    float r1 = glm::distance(glm::vec3(particles[0].position.x, particles[0].position.y, particles[0].position.z), centerOfMass);
    float r2 = glm::distance(glm::vec3(particles[1].position.x, particles[1].position.y, particles[1].position.z), centerOfMass);
    float r3 = glm::distance(glm::vec3(particles[2].position.x, particles[2].position.y, particles[2].position.z), centerOfMass);

    // Compute the initial velocities
    float G = 1.f; // Gravitational constant
    float v1 = sqrtf(G * totalMass / r1);
    float v2 = sqrtf(G * totalMass / r2);
    float v3 = sqrtf(G * totalMass / r3);
    glm::vec3 velocity1 = glm::normalize(glm::cross(vertex1 - centerOfMass, glm::vec3(0, 0, 1))) * v1;
    glm::vec3 velocity2 = glm::normalize(glm::cross(vertex2 - centerOfMass, glm::vec3(0, 0, 1))) * v2;
    glm::vec3 velocity3 = glm::normalize(glm::cross(vertex3 - centerOfMass, glm::vec3(0, 0, 1))) * v3;
    particles[0].setVelocity(velocity1 * 0.5f);
    particles[1].setVelocity(velocity2 * 0.5f);
    particles[2].setVelocity(velocity3 * 0.5f);


    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}


