//
// Created by marc on 5/03/23.
//

// ParticleSystem Abstract Class
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(size_t numParticles, InitializationType initType) {
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

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::generateCubeSystem() {
    for(size_t i{0}; i<this->particles.capacity(); i++){
        this->particles.emplace_back(glm::vec3(i, i, 0), glm::vec3(i, i, 0), 6);
    }
}

void ParticleSystem::generateGalaxySystem() {
    // TODO
    this->generateCubeSystem();
}

