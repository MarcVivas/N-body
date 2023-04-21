//
// Created by marc on 21/04/23.
//

#include "CPUTest.h"

CPUTest::CPUTest(ParticleSystem* particleSystem, ParticleSolver *particleSolver): Test() {
    this->particles = particleSystem;
    this->solver = particleSolver;
}

CPUTest::~CPUTest() {
    delete this->solver;
}
