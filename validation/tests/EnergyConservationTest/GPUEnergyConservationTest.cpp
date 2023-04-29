
#include "GPUEnergyConservationTest.h"
#include <iostream>

GPUEnergyConservationTest::GPUEnergyConservationTest(ParticleSystem* particleSystem, std::string& shaderPath, float stepSize, float softening) : GPUTest(particleSystem, shaderPath, stepSize, softening){
    this->energyCalculator = new EnergyCalculator(stepSize, softening);
}

GPUEnergyConservationTest::~GPUEnergyConservationTest() {
    delete this->energyCalculator;
}

void GPUEnergyConservationTest::runTest(const size_t iterations) {
    for (size_t i = 0; i < iterations; i++) {
        this->solver->updateParticlePositions(this->particles);
        this->updateParticleSystemBuffers();
        std::cout << "Iteration: " << i << '\n';
        this->energyCalculator->computeEnergy(this->particles);
    }
}




