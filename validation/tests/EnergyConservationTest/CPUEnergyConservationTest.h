//
// Created by marc on 21/04/23.
//
#include "../abstract/CPUTest/CPUTest.cpp"

#ifndef N_BODY_CPUENERGYCONSERVATIONTEST_H
#define N_BODY_CPUENERGYCONSERVATIONTEST_H


class CPUEnergyConservationTest: public CPUTest {
public:
    CPUEnergyConservationTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver);
    ~CPUEnergyConservationTest();
    virtual void runTest(const size_t iterations, const double stepSize) override;
protected:
    EnergyCalculator *energyCalculator;
};


#endif //N_BODY_CPUENERGYCONSERVATIONTEST_H
