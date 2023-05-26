#include "DeterministicCPUSolverTest.h"
#include <iostream>

DeterministicCPUSolverTest::DeterministicCPUSolverTest(ParticleSystem *particleSystem, ParticleSolver *particleSolver): CPUTest(particleSystem, particleSolver) {
    this->originalSystem = new ParticleSystem(particleSystem);


}

DeterministicCPUSolverTest::~DeterministicCPUSolverTest() {
    delete this->originalSystem;
}

void DeterministicCPUSolverTest::runTest(const size_t iterations) {
    ParticleSystem* after;
    int b = 0;

    for (unsigned int t = 0; t < 3; t++){

        // Update the particle system
        for (unsigned int i = 0; i < iterations; i++) {
            this->solver->updateParticlePositions(this->particles);
        }

        // Check if the particle system is the same as in the previous iteration

        if(t == 0){
            after = new ParticleSystem(this->particles);
        }

        b += compareParticleSystems(*this->particles, *after);

        if(t != 0){
            delete this->particles;
        }

        this->particles = new ParticleSystem(this->originalSystem);

    }

    if(b == 3){
        std::cout << "Passed\n";

    }
    else{
        std::cout << "Not Passed\n";
        std::cout << b;
    }
    delete after;

}

bool DeterministicCPUSolverTest::compareParticleSystems(ParticleSystem &ps1, ParticleSystem &ps2) {
    // Compare the number of particles
    if (ps1.size() != ps2.size()) {
        return false;
    }

    // Compare the properties of each particle
    unsigned int numParticles = ps1.size();
    for (unsigned int i = 0; i < numParticles; i++) {
        // Compare the positions
        if (ps1.getPositions()[i] != ps2.getPositions()[i]) {
            std::cout << i << " position" << std::endl ;
            std::cout << ps1.getPositions()[i].x << " " << ps1.getPositions()[i].y << " " << ps1.getPositions()[i].z << std::endl;
            std::cout << ps2.getPositions()[i].x << " " << ps2.getPositions()[i].y << " " << ps2.getPositions()[i].z << std::endl;
            return false;
        }

        // Compare the velocities
        if (ps1.getVelocities()[i] != ps2.getVelocities()[i]) {
            std::cout << i << " vel" << std::endl ;
            std::cout << ps1.getVelocities()[i].x << " " << ps1.getVelocities()[i].y << " " << ps1.getVelocities()[i].z << std::endl;
            std::cout << ps2.getVelocities()[i].x << " " << ps2.getVelocities()[i].y << " " << ps2.getVelocities()[i].z << std::endl;
            return false;
        }

        // Compare the accelerations
        if (ps1.getAccelerations()[i] != ps2.getAccelerations()[i]) {
            std::cout << i << " acc" << std::endl ;
            std::cout << ps1.getAccelerations()[i].x << " " << ps1.getAccelerations()[i].y << " " << ps1.getAccelerations()[i].z << std::endl;
            std::cout << ps2.getAccelerations()[i].x << " " << ps2.getAccelerations()[i].y << " " << ps2.getAccelerations()[i].z << std::endl;
            return false;
        }

        // Compare the masses
        if (ps1.getMasses()[i] != ps2.getMasses()[i]) {
            std::cout << i << " mass" << std::endl ;
            std::cout << ps1.getMasses()[i].x << " " << ps1.getMasses()[i].y << " " << ps1.getMasses()[i].z << std::endl;
            std::cout << ps2.getMasses()[i].x << " " << ps2.getMasses()[i].y << " " << ps2.getMasses()[i].z << std::endl;
            return false;
        }
    }

    return true;
}