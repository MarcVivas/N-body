//
// Created by marc on 5/03/23.
//
#include "../AbstractClass/ParticleSystem.h"

#ifndef N_BODY_PARTICLESYSTEMGPU_H
#define N_BODY_PARTICLESYSTEMGPU_H


class ParticleSystemGPU: public ParticleSystem{
public:
    void update(double deltaTime) override;
    void draw() override;

    ParticleSystemGPU(size_t numParticles, InitializationType initType);


};


#endif //N_BODY_PARTICLESYSTEMGPU_H