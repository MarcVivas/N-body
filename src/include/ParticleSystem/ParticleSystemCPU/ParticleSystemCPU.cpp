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
    glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this->renderShader->use();
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ParticleSystemCPU::update(double deltaTime) {
    //std::cout << "updateCPU " << deltaTime << '\n';
}


ParticleSystemCPU::ParticleSystemCPU(size_t numParticles, InitializationType initType): ParticleSystem(numParticles, initType){
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    std::string vertexShaderPath("../src/shaders/vertexShader.glsl");
#ifdef _WIN32
    std::replace(vertexShaderPath.begin(), vertexShaderPath.end(), '/', '\\');
#endif
    std::string fragmentShaderPath("../src/shaders/fragmentShader.glsl");
#ifdef _WIN32
    std::replace(fragmentShaderPath.begin(), fragmentShaderPath.end(), '/', '\\');
#endif

   this->renderShader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());


}