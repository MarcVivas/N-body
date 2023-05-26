#include "DeterministicGPUSolverTest.h"
#include <iostream>

DeterministicGPUSolverTest::DeterministicGPUSolverTest(ParticleSystem* particleSystem, std::string& positionsShaderPath, std::string& forcesShaderPath, float stepSize, float softening): GPUTest(particleSystem, positionsShaderPath, forcesShaderPath, stepSize, softening) {
    this->originalSystem = new ParticleSystem(particleSystem);


}

DeterministicGPUSolverTest::~DeterministicGPUSolverTest() {
    delete this->originalSystem;
}

void DeterministicGPUSolverTest::runTest(const size_t iterations) {
    ParticleSystem* after;
    int b = 0;

    for (unsigned int t = 0; t < 3; t++){

        // Update the particle system
        for (unsigned int i = 0; i < iterations; i++) {
            this->solver->updateParticlePositions(this->particles);
        }
        this->updateParticleSystemBuffers();


        // Check if the particle system is the same as in the previous iteration

        if(t == 0){
            after = new ParticleSystem(this->particles);
        }

        b += compareParticleSystems(*this->particles, *after);

        if(t != 0){
            delete this->particles;
        }

        this->particles = new ParticleSystem(this->originalSystem);
        this->resetSSBO();
    }

    if(b == 3){
        std::cout << "Passed\n";

    }
    else{
        std::cout << "Not Passed\n";
    }
    delete after;

}

bool DeterministicGPUSolverTest::compareParticleSystems(ParticleSystem &ps1, ParticleSystem &ps2) {
    // Compare the number of particles
    if (ps1.size() != ps2.size()) {
        return false;
    }

    // Compare the properties of each particle
    unsigned int numParticles = ps1.size();
    for (unsigned int i = 0; i < numParticles; i++) {
        // Compare the positions
        if (ps1.getPositions()[i] != ps2.getPositions()[i]) {
            return false;
        }

        // Compare the velocities
        if (ps1.getVelocities()[i] != ps2.getVelocities()[i]) {
            return false;
        }

        // Compare the accelerations
        if (ps1.getAccelerations()[i] != ps2.getAccelerations()[i]) {
            return false;
        }

        // Compare the masses
        if (ps1.getMasses()[i] != ps2.getMasses()[i]) {
            return false;
        }
    }

    return true;
}