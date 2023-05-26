#include "CPUTest.h"

#ifndef N_BODY_DETERMINISTICSOLVERTEST_H
#define N_BODY_DETERMINISTICSOLVERTEST_H


class DeterministicCPUSolverTest: public CPUTest{
public:
    DeterministicCPUSolverTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver);
    ~DeterministicCPUSolverTest();
    virtual void runTest(const size_t iterations) override;

protected:
    ParticleSystem *originalSystem;
    bool compareParticleSystems(ParticleSystem& ps1, ParticleSystem& ps2);

};


#endif //N_BODY_DETERMINISTICSOLVERTEST_H
