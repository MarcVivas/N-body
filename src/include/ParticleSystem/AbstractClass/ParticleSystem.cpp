//
// Created by marc on 5/03/23.
//

// ParticleSystem Abstract Class
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(size_t numParticles, InitializationType initType, ParticleSolver *particleSysSolver) :particleSolver(particleSysSolver){
    this->particles.reserve(numParticles);

    switch(initType){
        case InitializationType::CUBE:
            this->generateCubeSystem();
            break;
        case InitializationType::GALAXY:
            this->generateGalaxySystem();
            break;
    }
}

ParticleSystem::~ParticleSystem(){
    delete this->particleSolver;
}

void ParticleSystem::generateCubeSystem() {
    std::random_device randomDevice;
    std::mt19937 mt(randomDevice());
    std::uniform_real_distribution<float> randDist(-1, 1);

    for(size_t i = 0; i < particles.capacity(); i++){
        glm::vec3 particlePos =  glm::vec3(randDist(mt), randDist(mt), 0.f);   // glm::vec3(randDist(mt), randDist(mt), randDist(mt));
        glm::vec3 initialVel = glm::linearRand(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.3f, 0.1f, 0.5f));
        this->particles.emplace_back(particlePos, initialVel, 6);
    }
}

void ParticleSystem::generateGalaxySystem() {
    // TODO
    this->generateCubeSystem();
}

