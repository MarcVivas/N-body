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

    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->renderShader->use();

    // create transformations
    glm::mat4 model = glm::mat4(1.0f);

    // camera/view transformation
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));

    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)600 / (float)600, 0.1f, 100.0f);

    glm::mat4 modelViewProjection = projection * view * model;
    this->renderShader->setMat4("modelViewProjection", modelViewProjection);

    glDrawArrays(GL_POINTS, 0, this->particles.size());

    // Place a fence which will be removed when the draw command has finished
    this->lockParticlesBuffer();
    }

void ParticleSystemCPU::update(double deltaTime) {
    // Wait until the gpu is no longer using the particles buffer
    this->waitParticlesBuffer();

    this->particleSolver->updateParticlePositions(this->particles, deltaTime);

    this->copyParticlesToGPU();
}

ParticleSystemCPU::ParticleSystemCPU(ParticleSystemInitializer *particleSystemInitializer,
                                     ParticleSolver *particleSysSolver): ParticleSystem(particleSystemInitializer, particleSysSolver) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Persistent mapped buffer
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferStorage(GL_ARRAY_BUFFER, this->particles.size() * sizeof(Particle), &this->particles[0], GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    this->particlesPinnedMemory = (Particle *)glMapBufferRange(GL_ARRAY_BUFFER, 0, this->particles.size() * sizeof(Particle), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

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


void ParticleSystemCPU::copyParticlesToGPU() {
    std::memcpy(this->particlesPinnedMemory, &this->particles[0], this->particles.size() * sizeof(Particle));
}


void ParticleSystemCPU::lockParticlesBuffer()
{
    if( this->gSync )
    {
        glDeleteSync( this->gSync );
    }
    this->gSync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
}

void ParticleSystemCPU::waitParticlesBuffer()
{
    if( this->gSync )
    {
        while(true)
        {
            GLenum waitReturn = glClientWaitSync( this->gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1 );
            if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
                return;
        }
    }
}