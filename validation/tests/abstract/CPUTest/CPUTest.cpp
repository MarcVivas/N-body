
#include "CPUTest.h"

CPUTest::CPUTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver): Test() {
    this->particles = particleSystem;
    this->solver = particleSolver;
}

CPUTest::~CPUTest() {
    delete this->solver;
}
