
#include "CPUEnergyConservationTest.h"
#include <iostream>

CPUEnergyConservationTest::CPUEnergyConservationTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver, float stepSize, float softening): CPUTest(particleSystem, particleSolver) {
    this->energyCalculator = new EnergyCalculator(stepSize, softening);
}

CPUEnergyConservationTest::~CPUEnergyConservationTest(){
    delete this->energyCalculator;
}

void CPUEnergyConservationTest::runTest(const size_t iterations) {
    for (size_t i = 0; i < iterations; i++) {
        this->solver->updateParticlePositions(this->particles);
        std::cout << "Iteration: " << i << '\n';
        this->energyCalculator->computeEnergy(this->particles);
    }
}