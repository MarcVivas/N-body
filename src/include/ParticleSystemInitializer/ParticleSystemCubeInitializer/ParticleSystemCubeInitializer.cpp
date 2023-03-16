//
// Created by marc on 12/03/23.
//

#include "ParticleSystemCubeInitializer.h"

ParticleSystemCubeInitializer::ParticleSystemCubeInitializer(size_t numParticles) : totalParticles(numParticles){}

std::vector<Particle> ParticleSystemCubeInitializer::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    std::random_device randomDevice;
    std::mt19937 mt(randomDevice());
    std::uniform_real_distribution<float> randDist(0, worldDimensions.x);

    float length = glm::length(worldDimensions) / 20.f;

    for(size_t i = 0; i < totalParticles; i++){
        glm::vec3 particlePos =  glm::vec3(randDist(mt), randDist(mt), randDist(mt));
        glm::vec3 initialVel = glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
        particles[i] = Particle(particlePos, initialVel, 6);
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return particleVector;
}