#include <iostream>
#include <ctime>
#include <glm/glm.hpp>

#include "CPUEnergyConservationTest.h"
#include "GPUEnergyConservationTest.h"

#include "ParticleSystemCubeInitializer.h"
#include "ParticleSolverCPUParallel.h"
#include "ParticleSolverCPUSequential.h"

int main(){
    srand(static_cast<unsigned int>(std::time(nullptr)));
    ParticleSystemInitializer *initializer = new ParticleSystemCubeInitializer(1 + (rand() % 101));
    ParticleSystem *particleSystem = initializer->generateParticles(glm::vec3(1 + rand() % 700));
    delete initializer;

    glm::vec4* originalPositions = particleSystem->getPositions();
    glm::vec4* copiedPositions = new glm::vec4[particleSystem->size()];
    std::copy(originalPositions, originalPositions + particleSystem->size(), copiedPositions);

    glm::vec4* originalVelocities = particleSystem->getVelocities();
    glm::vec4* copiedVelocities = new glm::vec4[particleSystem->size()];
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), copiedVelocities);

    glm::vec4* originalAccelerations = particleSystem->getAccelerations();
    glm::vec4* copiedAccelerations = new glm::vec4[particleSystem->size()];
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), copiedAccelerations);


    Test *cpuSeqEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUSequential());
    Test *cpuParallelEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUParallel());

    std::string shaderPath = "../../src/shaders/ComputeShaders/updateParticles.glsl";
    Test *gpuEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, shaderPath);

    const size_t iterations = 100 + (rand() % 401);
    const double stepSize = 0.0005 + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (0.5 - 0.0005)));

    std::cout << "Energy test: CPU sequential\n";
    cpuSeqEnergyConservationTest->runTest(iterations, stepSize);

    std::cout << "Energy test: CPU parallel\n";
    particleSystem->setAccelerations(copiedAccelerations);
    particleSystem->setPositions(copiedPositions);
    particleSystem->setVelocities(copiedVelocities);
    cpuParallelEnergyConservationTest->runTest(iterations, stepSize);

    std::cout << "Energy test: GPU\n";
    gpuEnergyConservationTest->runTest(iterations, stepSize);

    delete cpuSeqEnergyConservationTest;
    delete cpuParallelEnergyConservationTest;
    delete gpuEnergyConservationTest;
    delete particleSystem;
    delete[] copiedVelocities;
    delete[] copiedPositions;
    delete[] copiedAccelerations;
}