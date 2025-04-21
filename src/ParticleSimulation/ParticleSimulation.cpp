#include "ParticleSimulation.h"


ParticleSimulation::ParticleSimulation(
    std::unique_ptr<ParticleSystemInitializer> particleSystemInitializer,
    std::unique_ptr<ParticleSolver> particleSysSolver,
    glm::vec3 worldDim,
    glm::vec2 windowDim
)
{
    this->particleSolver = std::move(particleSysSolver);
    this->particleSystem = particleSystemInitializer->generateParticles(worldDim, particleSolver->usesGPU());
    this->particleDrawer = std::make_unique<ParticleDrawer> (worldDim, windowDim);
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
