#include "Test.h"
#include "ParticleSystem.h"
#include "ParticleSolver.h"

#ifndef N_BODY_CPUTEST_H
#define N_BODY_CPUTEST_H


class CPUTest: public Test {
public:
    CPUTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver);
    virtual ~CPUTest();
    virtual void runTest(const size_t iterations, const double stepSize) = 0;

protected:
    ParticleSystem *particles;
    ParticleSolver *solver;
};


#endif //N_BODY_CPUTEST_H
