//
// Created by marc on 5/03/23.
//

#include "ParticleSystemCPU.h"

ParticleSystemCPU::~ParticleSystemCPU() {
    delete this->renderShader;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void ParticleSystemCPU::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->particles.size() * sizeof(Particle), &this->particles[0], GL_DYNAMIC_DRAW);
    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this->renderShader->use();
    glDrawArrays(GL_POINTS, 0, this->particles.size());
}

void ParticleSystemCPU::update(double deltaTime) {
    for(Particle &particle: this->particles){
        glm::vec3 newAcceleration = glm::vec3(0,0,0);
        particle.updatePosition(deltaTime, newAcceleration);

    }
}


ParticleSystemCPU::ParticleSystemCPU(size_t numParticles, InitializationType initType): ParticleSystem(numParticles, initType){

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->particles.size() * sizeof(Particle), &this->particles[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)offsetof(Particle, velocity));

    std::string vertexShaderPath("../src/shaders/vertexShader.glsl");
    std::string fragmentShaderPath("../src/shaders/fragmentShader.glsl");

#ifdef _WIN32
    std::replace(vertexShaderPath.begin(), vertexShaderPath.end(), '/', '\\');
    std::replace(fragmentShaderPath.begin(), fragmentShaderPath.end(), '/', '\\');
#endif

   this->renderShader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());


}