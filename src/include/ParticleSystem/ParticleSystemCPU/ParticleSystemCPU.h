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
    ParticleSystemCPU(size_t numParticles, InitializationType initType);
protected:
    Shader *renderShader;
    unsigned int VAO;
    unsigned int VBO;
};


#endif //N_BODY_PARTICLESYSTEMCPU_H
