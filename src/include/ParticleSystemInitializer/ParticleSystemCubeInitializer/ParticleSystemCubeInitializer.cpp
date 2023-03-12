//
// Created by marc on 12/03/23.
//

#include "ParticleSystemCubeInitializer.h"

ParticleSystemCubeInitializer::ParticleSystemCubeInitializer(size_t numParticles) : totalParticles(numParticles){}

std::vector<Particle> ParticleSystemCubeInitializer::generateParticles() {
    Particle* particles = new Particle[this->totalParticles];

    std::random_device randomDevice;
    std::mt19937 mt(randomDevice());
    std::uniform_real_distribution<float> randDist(-1, 1);

    for(size_t i = 0; i < totalParticles; i++){
        glm::vec3 particlePos =  glm::vec3(randDist(mt), randDist(mt), 0.f);   // glm::vec3(randDist(mt), randDist(mt), randDist(mt));
        glm::vec3 initialVel = glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.3f, 0.1f, 0.5f));
        particles[i] = Particle(particlePos, initialVel, 6);
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return particleVector;
}