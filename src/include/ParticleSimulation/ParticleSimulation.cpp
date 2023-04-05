//
// Created by marc on 5/03/23.
//

// ParticleSystem Abstract Class
#include "ParticleSimulation.h"

ParticleSimulation::~ParticleSimulation() {
    delete this->renderShader;
    delete this->camera;
    delete this->particleSolver;
    delete this->bloom;
    delete this->finalRenderShader;
    delete this->blurShader;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void ParticleSimulation::draw() {
    
    // Render the normal scene and extract the bright particles
    this->bloom->useFrameBuffer();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->renderShader->use();
    this->renderShader->setMat4("modelViewProjection", this->camera->getModelViewProjection());
    this->renderShader->setVec3("cameraPos", this->camera->getPosition());
    glDrawArrays(GL_POINTS, 0, this->particles.size());


    // Blur bright particles with two-pass Gaussian Blur
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 2;
    this->blurShader->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        this->bloom->bindPingPongBuffer(horizontal);
        this->blurShader->setInt("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? this->bloom->getBrightPointsScene() : this->bloom->getPingPongTexture(!horizontal));  // bind texture of other framebuffer (or scene if first iteration)
        glBindVertexArray(this->screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        horizontal = !horizontal;
        if (first_iteration){
            first_iteration = false;
        }
    }


    // Render the final scene
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    this->finalRenderShader->use();
    glBindVertexArray(this->screenVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->bloom->getNormalScene());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->bloom->getPingPongTexture(!horizontal));	// use the color attachment texture as the texture of the quad plane

    glDrawArrays(GL_TRIANGLES, 0, 6);


    this->lockParticlesBuffer();
}

void ParticleSimulation::update(double deltaTime) {
    this->particleSolver->updateParticlePositions(this->particles, this->particlesPositions, this->particlesVelocities, deltaTime);
    this->waitParticlesBuffer();
}


ParticleSimulation::ParticleSimulation(ParticleSystemInitializer *particleSystemInitializer,
                               ParticleSolver *particleSysSolver, glm::vec3 worldDim, glm::vec2 windowDim){

    this->worldDimensions = worldDim;
    this->particleSolver = particleSysSolver;
    this->camera = new Camera(windowDim, worldDim);
    this->bloom = new Bloom(windowDim, 1.f, true);
    this->particles = particleSystemInitializer->generateParticles(worldDimensions);


    this->createBuffers(this->particleSolver->usesGPU());


    this->renderShader = new VertexFragmentShader("../src/shaders/vertexShader.glsl", "../src/shaders/fragmentShader.glsl");
    this->renderShader->use();
    this->renderShader->setFloat("worldSize", glm::length(this->worldDimensions));


    this->finalRenderShader = new VertexFragmentShader("../src/shaders/finalRender_vs.glsl", "../src/shaders/finalRender_fs.glsl");
    this->finalRenderShader->use();
    this->finalRenderShader->setInt("scene", 0);
    this->finalRenderShader->setInt("bloomBlur", 1);
    this->finalRenderShader->setFloat("exposure", this->bloom->getExposure());

    // screen quad VAO
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), &textureVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    this->blurShader = new VertexFragmentShader("../src/shaders/blur_vs.glsl", "../src/shaders/blur_fs.glsl");
    this->blurShader->use();
    this->blurShader->setInt("image", 0);

}


void ParticleSimulation::copyParticlesToGPU() {
    std::memcpy(this->particlesPositions, &this->getPositions()[0], this->particles.size() * sizeof(glm::vec4));
    std::memcpy(this->particlesVelocities, &this->getVelocities()[0], this->particles.size() * sizeof(glm::vec4));
}


void ParticleSimulation::createBuffers(bool usesGPU) {
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(this->VAO);


    // Shader storage buffer objects

    // This SSBO stores particles positions
    glGenBuffers(1, &this->postitions_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->postitions_SSBO);

    // This SSBO stores particles velocities
    glGenBuffers(1, &this->velocities_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->velocities_SSBO);

    // This SSBO stores particles accelerations
    glGenBuffers(1, &this->accelerations_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->accelerations_SSBO);


    if(usesGPU){
        this->configureGpuBuffers();
    }
    else{
        this->configureCpuBuffers();
    }

}

void ParticleSimulation::configureGpuBuffers() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getPositions()[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getVelocities()[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getAccelerations()[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/**
 * Creates persistent mapped shader storage buffer objects
 */
void ParticleSimulation::configureCpuBuffers() {
    GLbitfield bufferStorageFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getPositions()[0], bufferStorageFlags);
    this->particlesPositions = (glm::vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particles.size(), bufferStorageFlags);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getVelocities()[0], bufferStorageFlags);
    this->particlesVelocities = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particles.size(), bufferStorageFlags);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles.size(), &this->getAccelerations()[0], bufferStorageFlags);
    this->particlesAccelerations = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particles.size(), bufferStorageFlags);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}


void ParticleSimulation::lockParticlesBuffer()
{
    if( this->gSync )
    {
        glDeleteSync( this->gSync );
    }
    this->gSync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
}

void ParticleSimulation::waitParticlesBuffer()
{
    // Insert a memory barrier to ensure synchronization between CPU and GPU
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
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

Camera* ParticleSimulation::getCamera(){
    return this->camera;
}


std::vector<glm::vec4> ParticleSimulation::getPositions() {
    std::vector<glm::vec4> positions(this->particles.size());

    std::transform(this->particles.begin(), this->particles.end(), positions.begin(),
                   [](const Particle& p) { return p.position; });

    return positions;
}

std::vector<glm::vec4> ParticleSimulation::getVelocities() {
    std::vector<glm::vec4> velocities(this->particles.size());

    std::transform(particles.begin(), particles.end(), velocities.begin(),
                   [](const Particle& p) { return p.velocity; });

    return velocities;
}

std::vector<glm::vec4> ParticleSimulation::getAccelerations() {
    std::vector<glm::vec4> accelerations(this->particles.size());

    std::transform(particles.begin(), particles.end(), accelerations.begin(),
                   [](const Particle& p) { return p.acceleration; });

    return accelerations;
}