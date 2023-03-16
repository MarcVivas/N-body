//
// Created by marc on 5/03/23.
//

// ParticleSystem Abstract Class
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(ParticleSystemInitializer *particleSystemInitializer,
                               ParticleSolver *particleSysSolver, glm::vec3 worldDim) :particleSolver(particleSysSolver), worldDimensions(worldDim) {
    this->particles = particleSystemInitializer->generateParticles(worldDimensions);
}


ParticleSystem::~ParticleSystem(){
    delete this->particleSolver;
}

