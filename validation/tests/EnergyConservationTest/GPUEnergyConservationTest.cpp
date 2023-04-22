
#include "GPUEnergyConservationTest.h"
#include <iostream>

GPUEnergyConservationTest::GPUEnergyConservationTest(ParticleSystem* particleSystem, std::string& shaderPath) : GPUTest(particleSystem, shaderPath){
    this->energyCalculator = new EnergyCalculator();
}

GPUEnergyConservationTest::~GPUEnergyConservationTest() {
    delete this->energyCalculator;
}

void GPUEnergyConservationTest::runTest(const size_t iterations, const double stepSize) {
    for (size_t i = 0; i < iterations; i++) {
        this->solver->updateParticlePositions(this->particles, stepSize);
        this->updateParticleSystemBuffers();
        std::cout << "Iteration: " << i << '\n';
        std::cout << "Step size: " << stepSize << '\n';
        this->energyCalculator->computeEnergy(this->particles, this->solver->getSquaredSoftening());
    }
}




