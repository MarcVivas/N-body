#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ParticleSystem.h"
#include "ParticleSystemCubeInitializer.h"
#include "ParticleSystemGalaxyInitializer.h"
#include "ParticleSolver.h"
#include "ParticleDrawer.h"
#include "ParticleSystemSaver.h"

#ifndef PARTICLESIMULATION_H
#define PARTICLESIMULATION_H
class ParticleSimulation {
public:
    virtual void update();
    virtual void draw();
    virtual void saveCurrentState(int iteration);
    virtual ParticleDrawer* getParticleDrawer();
    ParticleSimulation(ParticleSystemInitializer *particleSystemInitializer, ParticleSolver *particleSysSolver, glm::vec3 worldDim, glm::vec2 windowDim, std::string saveFileName);
    ~ParticleSimulation();

protected:
    ParticleSolver *particleSolver;
    ParticleSystem *initialParticleSystem;
    ParticleDrawer *particleDrawer;
    ParticleSystem *particleSystem;
    ParticleSystemSaver *saver;
    GLuint VAO;
    GLuint VBO;
    GLsync gSync = nullptr;
    GLuint postitions_SSBO, velocities_SSBO, accelerations_SSBO, masses_SSBO, forces_SSBO, nodes_SSBO;

    void lockParticlesBuffer();
    void waitParticlesBuffer();
    void createBuffers(bool usesGPU);
    void configureGpuBuffers();
    void configureCpuBuffers();
    void configureBHBuffers();

    virtual void updateGPUParticleSystem();

};
#endif // PARTICLESIMULATION_H
