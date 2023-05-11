#include "ParticleSystemCubeInitializer.h"
#include <random>
#include <glm/gtc/random.hpp>
#include <chrono>

ParticleSystemCubeInitializer::ParticleSystemCubeInitializer(size_t numParticles) : totalParticles(numParticles){}

ParticleSystem* ParticleSystemCubeInitializer::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> randDist(0, worldDimensions.x);
    std::uniform_real_distribution<float> mass(0.8, 1.3);


    float length = glm::length(worldDimensions) / 20.f;

    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 1.2;

    for(size_t i = 0; i < totalParticles; i++){
        glm::vec3 particlePos = (worldDimensions.z == 0) ? glm::vec3(randDist(mt), randDist(mt), 0.f) : glm::vec3(randDist(mt), randDist(mt), randDist(mt));
        glm::vec3 initialVel = (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
        particles[i] = Particle(particlePos, initialVel, particleMass*mass(mt));
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}