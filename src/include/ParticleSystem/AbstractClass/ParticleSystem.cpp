//
// Created by marc on 5/03/23.
//

// ParticleSystem Abstract Class
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(size_t numParticles) {
    std::cout << "Constructor " << numParticles << " particles \n";
}

ParticleSystem::~ParticleSystem() = default;
