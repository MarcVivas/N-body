
#include "CPUTest.h"
#include "EnergyCalculator.h"

#ifndef N_BODY_CPUENERGYCONSERVATIONTEST_H
#define N_BODY_CPUENERGYCONSERVATIONTEST_H


class CPUEnergyConservationTest: public CPUTest {
public:
    CPUEnergyConservationTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver, float stepSize, float softening);
    ~CPUEnergyConservationTest();
    virtual void runTest(const size_t iterations) override;
protected:
    EnergyCalculator *energyCalculator;
};


#endif //N_BODY_CPUENERGYCONSERVATIONTEST_H
