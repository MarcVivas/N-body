#include "ParticleSimulation.h"

ParticleSimulation::ParticleSimulation(ParticleSystemInitializer *particleSystemInitializer,
                                       ParticleSolver *particleSysSolver, glm::vec3 worldDim, glm::vec2 windowDim, std::string saveFileName){
    this->particleSolver = particleSysSolver;
    this->particleDrawer = new ParticleDrawer(worldDim, windowDim);
    this->particleSystem = particleSystemInitializer->generateParticles(worldDim);
    this->saver = new ParticleSystemSaver(saveFileName);
    saver->saveInitialState(particleSystem);
    this->createBuffers(this->particleSolver->usesGPU());
}


ParticleSimulation::~ParticleSimulation() {
    updateGPUParticleSystem();
    saver->saveFinalState(particleSystem);
    delete this->saver;
    delete this->particleDrawer;
    delete this->particleSolver;
    delete this->particleSystem;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void ParticleSimulation::draw() {
    this->particleDrawer->draw(this->particleSystem->size());
    this->lockParticlesBuffer();
}

void ParticleSimulation::update() {
    this->particleSolver->updateParticlePositions(this->particleSystem);
    this->waitParticlesBuffer();
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

    // This SSBO stores particles accelerations
    glGenBuffers(1, &this->masses_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->masses_SSBO);

    // This SSBO stores particles accelerations
    glGenBuffers(1, &this->forces_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, this->forces_SSBO);


    if(usesGPU){
        this->configureGpuBuffers();
    }
    else{
        this->configureCpuBuffers();
    }

}

void ParticleSimulation::configureGpuBuffers() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getPositions(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getVelocities(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getAccelerations(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->masses_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getMasses(), GL_STATIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->forces_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getForces(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/**
 * Creates persistent mapped shader storage buffer objects
 */
void ParticleSimulation::configureCpuBuffers() {
    GLbitfield bufferStorageFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getPositions(), bufferStorageFlags);
    glm::vec4* positions = (glm::vec4*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setPositions(positions);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getVelocities(), bufferStorageFlags);
    glm::vec4* velocities = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setVelocities(velocities);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getAccelerations(), bufferStorageFlags);
    glm::vec4* accelerations = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setAccelerations(accelerations);
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


void ParticleSimulation::updateGPUParticleSystem() {
    if(this->particleSolver->usesGPU()){
        int numParticles = this->particleSystem->size();


        // Bind the velocity SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
        // Map the velocity SSBO memory to CPU-accessible memory
        glm::vec4* velocities = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        glm::vec4* copiedVelocities = new glm::vec4[numParticles];
        std::copy(velocities, velocities + numParticles, copiedVelocities);
        this->particleSystem->setVelocities(copiedVelocities);
        // Unmap the velocity SSBO
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


        // Bind the position SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
        // Map the position SSBO memory to CPU-accessible memory
        glm::vec4* positions = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        glm::vec4* copiedPositions = new glm::vec4[numParticles];
        std::copy(positions, positions + numParticles, copiedPositions);
        this->particleSystem->setPositions(copiedPositions);
        // Unmap the position SSBO
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        // Bind the acceleration SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
        // Map the acceleration SSBO memory to CPU-accessible memory
        glm::vec4* accelerations = static_cast<glm::vec4*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));
        glm::vec4* copiedAccelerations = new glm::vec4[numParticles];
        std::copy(accelerations, accelerations + numParticles, copiedAccelerations);
        this->particleSystem->setAccelerations(copiedAccelerations);
        // Unmap the acceleration SSBO
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        // Unbind the shader storage buffers
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}


void ParticleSimulation::saveCurrentState(int iteration) {
    updateGPUParticleSystem();
    this->saver->saveCurrentState(iteration, particleSystem);
}


ParticleDrawer *ParticleSimulation::getParticleDrawer() {
    return this->particleDrawer;
}