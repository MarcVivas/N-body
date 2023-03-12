//
// Created by marc on 5/03/23.
//
#include "../AbstractClass/ParticleSystem.h"

#ifndef N_BODY_PARTICLESYSTEMCPU_H
#define N_BODY_PARTICLESYSTEMCPU_H


class ParticleSystemCPU: public ParticleSystem{
public:
    void update(double deltaTime) override;
    void draw() override;

    virtual ~ParticleSystemCPU();
    ParticleSystemCPU(ParticleSystemInitializer *particleSystemInitializer, ParticleSolver *particleSysSolver);
protected:
    Shader *renderShader;
    unsigned int VAO;
    unsigned int VBO;
    GLsync gSync = nullptr;
    Particle* particlesPinnedMemory;

    void lockParticlesBuffer();
    void waitParticlesBuffer();
    void copyParticlesToGPU();
};


#endif //N_BODY_PARTICLESYSTEMCPU_H
