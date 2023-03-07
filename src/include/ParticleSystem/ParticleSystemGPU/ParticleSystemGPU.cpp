//
// Created by marc on 5/03/23.
//

#include "ParticleSystemGPU.h"

void ParticleSystemGPU::draw() {
    std::cout << "draw GPU \n";
}

void ParticleSystemGPU::update(double deltaTime) {
    std::cout << "update GPU " << deltaTime << '\n';
}

ParticleSystemGPU::ParticleSystemGPU(size_t numParticles, InitializationType initType): ParticleSystem(numParticles, initType){
    std::cout << "constructor GPU " << numParticles << " particles\n";
}