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


ParticleSystemCPU::ParticleSystemCPU(size_t numParticles, InitializationType initType): ParticleSystem(numParticles, initType){
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    


}