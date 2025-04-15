

#ifndef PARTICLESIMULATION_H
#define PARTICLESIMULATION_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ParticleSystem.h"
#include "ParticleSystemCubeInitializer.h"
#include "ParticleSystemGalaxyInitializer.h"
#include "ParticleSolver.h"
#include "ParticleDrawer.h"
#include "OpenGLBuffer.h"
#include <memory>

class ParticleSimulation {
public:
    virtual void update();
    virtual void draw(float dt);
    ParticleDrawer* getParticleDrawer() const {
		return this->particleDrawer.get();
    }
    ParticleSimulation(
        std::unique_ptr<ParticleSystemInitializer> particleSystemInitializer,
        std::unique_ptr<ParticleSolver> particleSysSolver,
        glm::vec3 worldDim,
        glm::vec2 windowDim
    );
    ~ParticleSimulation();

protected:
    std::unique_ptr<ParticleSolver> particleSolver;
	std::unique_ptr<ParticleDrawer> particleDrawer;
    std::unique_ptr<ParticleSystem> particleSystem;
    GLsync gSync = nullptr;
    OpenGLBuffer positions_SSBO, velocities_SSBO, accelerations_SSBO, masses_SSBO, forces_SSBO, nodes_SSBO;

    void lockParticlesBuffer();
    void waitParticlesBuffer();
    void createBuffers(bool usesGPU);
    void configureGpuBuffers();
    void configureCpuBuffers();

};
#endif // PARTICLESIMULATION_H
