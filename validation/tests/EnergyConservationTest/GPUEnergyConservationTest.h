
#include "GPUTest.h"
#include "EnergyCalculator.h"

#ifndef N_BODY_GPUENERGYCONSERVATIONTEST_H
#define N_BODY_GPUENERGYCONSERVATIONTEST_H


class GPUEnergyConservationTest: public GPUTest {
public:
    GPUEnergyConservationTest(ParticleSystem* particleSystem, std::string& shaderPath, float stepSize, float softening);
    ~GPUEnergyConservationTest();
    void runTest(const size_t iterations);
protected:
    EnergyCalculator *energyCalculator;

};


#endif //N_BODY_GPUENERGYCONSERVATIONTEST_H
