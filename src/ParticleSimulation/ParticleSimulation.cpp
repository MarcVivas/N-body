#include "ParticleSimulation.h"


ParticleSimulation::ParticleSimulation(
    std::unique_ptr<ParticleSystemInitializer> particleSystemInitializer,
    std::unique_ptr<ParticleSolver> particleSysSolver,
    glm::vec3 worldDim,
    glm::vec2 windowDim
)
{
    this->particleSolver = std::move(particleSysSolver);
    this->particleDrawer = std::make_unique<ParticleDrawer> (worldDim, windowDim);
    this->particleSystem = particleSystemInitializer->generateParticles(worldDim);
    this->createBuffers(this->particleSolver->usesGPU());
    
}


ParticleSimulation::~ParticleSimulation() = default;

void ParticleSimulation::draw(const float dt) {
    this->particleDrawer->draw(this->particleSystem->size(), dt);
    if (particleSolver->usesGPU()) return;
    this->lockParticlesBuffer();
}

void ParticleSimulation::update() {
    this->particleSolver->updateParticlePositions(this->particleSystem.get());
    if (particleSolver->usesGPU()) return;
    this->waitParticlesBuffer();
}

void ParticleSimulation::createBuffers(bool usesGPU) {
    if (usesGPU) {
        this->configureGpuBuffers();
    }
    else{
        this->configureCpuBuffers();
    }
}


void ParticleSimulation::configureGpuBuffers() {
    positions_SSBO.createBufferData(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getPositions(), 0, GL_DYNAMIC_DRAW);
	velocities_SSBO.createBufferData(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getVelocities(), 1, GL_DYNAMIC_DRAW);
    accelerations_SSBO.createBufferData(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getAccelerations(), 2, GL_DYNAMIC_DRAW);
    masses_SSBO.createBufferData(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getMasses(), 3, GL_STATIC_DRAW);
    forces_SSBO.createBufferData(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getForces(), 4, GL_DYNAMIC_DRAW);
}

/**
 * Creates persistent mapped shader storage buffer objects
 */
void ParticleSimulation::configureCpuBuffers() {
    GLbitfield bufferStorageFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    positions_SSBO.createBufferStorage(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getPositions(), 0, bufferStorageFlags);
	glm::vec4* positions = (glm::vec4*) positions_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setPositions(positions);

	velocities_SSBO.createBufferStorage(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getVelocities(), 1, bufferStorageFlags);
    glm::vec4* velocities = (glm::vec4*)velocities_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setVelocities(velocities);

	accelerations_SSBO.createBufferStorage(sizeof(glm::vec4) * this->particleSystem->size(), this->particleSystem->getAccelerations(), 2, bufferStorageFlags);
    glm::vec4* accelerations = (glm::vec4*)accelerations_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->particleSystem->size(), bufferStorageFlags);
    this->particleSystem->setAccelerations(accelerations);
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
    if (gSync) {
        GLenum waitReturn;
        do {
            waitReturn = glClientWaitSync(gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000); // 1-second timeout
        } while (waitReturn == GL_TIMEOUT_EXPIRED);
    }
}
