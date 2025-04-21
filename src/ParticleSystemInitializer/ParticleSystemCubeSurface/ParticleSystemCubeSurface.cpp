#include "ParticleSystemCubeSurface.h"
#include <random>
#include <glm/gtc/random.hpp>
#include <chrono>

ParticleSystemCubeSurface::ParticleSystemCubeSurface(size_t numParticles) : totalParticles(numParticles){}

std::unique_ptr<ParticleSystem> ParticleSystemCubeSurface::generateParticles(glm::vec3 worldDimensions, const bool usesGPU) {
    std::vector<Particle> particles;
    particles.reserve(this->totalParticles);

    std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> randDist(-worldDimensions.x / 2.f, worldDimensions.x / 2.f);
    std::uniform_real_distribution<float> mass(0.8, 1.3);

    float length = glm::length(worldDimensions) / 20.f;
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 1.2;

    for(size_t i = 0; i < totalParticles; i++){
        glm::vec3 particlePos;
        int surface = rand() % 6; // choose a random surface
        switch (surface) {
            case 0: // left
                particlePos = glm::vec3(-worldDimensions.x / 2.f, randDist(mt), randDist(mt));
                break;
            case 1: // right
                particlePos = glm::vec3(worldDimensions.x / 2.f, randDist(mt), randDist(mt));
                break;
            case 2: // bottom
                particlePos = glm::vec3(randDist(mt), -worldDimensions.y / 2.f, randDist(mt));
                break;
            case 3: // top
                particlePos = glm::vec3(randDist(mt), worldDimensions.y / 2.f, randDist(mt));
                break;
            case 4: // front
                particlePos = glm::vec3(randDist(mt), randDist(mt), -worldDimensions.z / 2.f);
                break;
            case 5: // back
                particlePos = glm::vec3(randDist(mt), randDist(mt), worldDimensions.z / 2.f);
                break;
        }
        particlePos += 0.5f * worldDimensions;
        glm::vec3 initialVel = (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
		particles.push_back(Particle(particlePos, initialVel, particleMass * mass(mt)));
    }

    return std::make_unique<ParticleSystem>(particles, usesGPU);
}