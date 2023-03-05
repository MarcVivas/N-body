//
// Created by marc on 5/03/23.
//

// ParticleSystem AbstractClass

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
class ParticleSystem {
public:
    virtual void update(double deltaTime) = 0;
    virtual void draw() = 0;

    ParticleSystem(size_t numParticles);
    virtual ~ParticleSystem();
};
#endif // PARTICLESYSTEM_H
