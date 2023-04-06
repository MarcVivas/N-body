//
// Created by marc on 5/03/23.
//

#include "ParticleSimulation.h"

ParticleSimulation::~ParticleSimulation() {
    delete this->particleDrawer;
    delete this->particleSolver;
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void ParticleSimulation::draw() {
    this->particleDrawer->draw(this->particles.size());
    this->lockParticlesBuffer();
}

void ParticleSimulation::update(double deltaTime) {
    this->particleSolver->updateParticlePositions(this->particles, this->particlesPositions, this->particlesVelocities, deltaTime);
    this->waitParticlesBuffer();
}


ParticleSimulation::ParticleSimulation(ParticleSystemInitializer *particleSystemInitializer,
                               ParticleSolver *particleSysSolver, glm::vec3 worldDim, glm::vec2 windowDim){

    this->particleSolver = particleSysSolver;
    this->particleDrawer = new ParticleDrawer(worldDim, windowDim);
    this->particles = particleSystemInitializer->generateParticles(worldDim);
    this->createBuffers(this->particleSolver->usesGPU());
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


ParticleDrawer *ParticleSimulation::getParticleDrawer() {
    return this->particleDrawer;
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