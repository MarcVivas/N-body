#include <Octree.h>

#include "ParticleSystem.h"

#ifndef N_BODY_PARTICLESOLVER_H
#define N_BODY_PARTICLESOLVER_H


class ParticleSolver {
public:
    virtual ~ParticleSolver() = default;
    ParticleSolver() = default;
    virtual void updateParticlePositions(ParticleSystem *particles) = 0;
    virtual float getSquaredSoftening() = 0;
    virtual bool usesGPU() = 0;
    virtual bool usesBH() = 0;
    virtual Octree* getOctree() = 0;

};


#endif //N_BODY_PARTICLESOLVER_H
