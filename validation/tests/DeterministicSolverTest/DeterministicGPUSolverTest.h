#include "GPUTest.h"

#ifndef N_BODY_DETERMINISTICSOLVERGPUTEST_H
#define N_BODY_DETERMINISTICSOLVERGPUTEST_H


class DeterministicGPUSolverTest: public GPUTest{
public:
    DeterministicGPUSolverTest(ParticleSystem* particleSystem, std::string& positionsShaderPath, std::string& forcesShaderPath, float stepSize, float softening);
    ~DeterministicGPUSolverTest();
    virtual void runTest(const size_t iterations) override;

protected:
    ParticleSystem *originalSystem;
    bool compareParticleSystems(ParticleSystem& ps1, ParticleSystem& ps2);

};


#endif //N_BODY_DETERMINISTICSOLVERGPUTEST_H
