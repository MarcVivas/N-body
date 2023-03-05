//
// Created by marc on 5/03/23.
//

#include "ParticleSystemCPU.h"

void ParticleSystemCPU::draw() {
    std::cout << "drawCPU \n";
}

void ParticleSystemCPU::update(double deltaTime) {
    std::cout << "updateCPU " << deltaTime << '\n';
}


ParticleSystemCPU::ParticleSystemCPU(size_t numParticles): ParticleSystem(numParticles){
    std::cout << "constructor CPU " << numParticles << " particles\n";
}