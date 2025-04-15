

#ifndef N_BODY_PARTICLESOLVER_H
#define N_BODY_PARTICLESOLVER_H
#include "ParticleSystem.h"


class ParticleSolver {
public:
    virtual ~ParticleSolver() = default;
    ParticleSolver() = default;
    virtual void updateParticlePositions(ParticleSystem *particles) = 0;
    virtual float getSquaredSoftening() = 0;
    virtual bool usesGPU() = 0;
};


#endif //N_BODY_PARTICLESOLVER_H
