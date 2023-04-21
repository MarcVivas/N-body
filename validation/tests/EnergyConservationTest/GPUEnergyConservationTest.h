//
// Created by marc on 20/04/23.
//

#include "../abstract/GPUTest/GPUTest.cpp"

#ifndef N_BODY_GPUENERGYCONSERVATIONTEST_H
#define N_BODY_GPUENERGYCONSERVATIONTEST_H


class GPUEnergyConservationTest: public GPUTest {
public:
    GPUEnergyConservationTest(ParticleSystem* particleSystem, std::string& shaderPath);
    ~GPUEnergyConservationTest();
    void runTest(const size_t iterations, const double stepSize);
protected:
    EnergyCalculator *energyCalculator;

};


#endif //N_BODY_GPUENERGYCONSERVATIONTEST_H
