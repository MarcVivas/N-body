
#include "CPUEnergyConservationTest.h"
#include <iostream>

CPUEnergyConservationTest::CPUEnergyConservationTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver): CPUTest(particleSystem, particleSolver) {
    this->energyCalculator = new EnergyCalculator();
}

CPUEnergyConservationTest::~CPUEnergyConservationTest(){
    delete this->energyCalculator;
}

void CPUEnergyConservationTest::runTest(const size_t iterations, const double stepSize) {
    for (size_t i = 0; i < iterations; i++) {
        this->solver->updateParticlePositions(this->particles, stepSize);
        std::cout << "Iteration: " << i << '\n';
        std::cout << "Step size: " << stepSize << '\n';
        this->energyCalculator->computeEnergy(this->particles, this->solver->getSquaredSoftening());
    }
}