#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include "ArgumentsParser.h"

#include "CPUEnergyConservationTest.h"
#include "GPUEnergyConservationTest.h"

#include "ParticleSystemCubeInitializer.h"
#include "ParticleSolverCPUParallel.h"
#include "ParticleSolverCPUSequential.h"

int main(int argc, char *argv[]){

    // Get the arguments
    ArgumentsParser args(argc, argv);

    srand(static_cast<unsigned int>(std::time(nullptr)));
    ParticleSystemInitializer *initializer = new ParticleSystemCubeInitializer(4 + (rand() % 6));
    ParticleSystem *particleSystem = initializer->generateParticles(glm::vec3(5, 5, 5));
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

    const size_t iterations = 30000;

    Test *cpuSeqEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUSequential(args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());
    Test *cpuParallelEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUParallel(args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());

    std::string positionsShaderPath = "../../src/shaders/ComputeShaders/updateParticles.glsl";
    std::string forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculation.glsl";



    std::cout << "Energy test: CPU sequential\n";
    cpuSeqEnergyConservationTest->runTest(iterations);
    delete cpuSeqEnergyConservationTest;

    std::cout << "Energy test: CPU parallel\n";
    originalPositions = copiedPositions;
    glm::vec4* newCopiedPositions = new glm::vec4[particleSystem->size()];
    std::copy(originalPositions, originalPositions + particleSystem->size(), newCopiedPositions);

    originalVelocities = copiedVelocities;
    glm::vec4* newCopiedVelocities = new glm::vec4[particleSystem->size()];
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), newCopiedVelocities);

    originalAccelerations = copiedAccelerations;
    glm::vec4* newCopiedAccelerations = new glm::vec4[particleSystem->size()];
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), newCopiedAccelerations);
    particleSystem->setAccelerations(newCopiedAccelerations);
    particleSystem->setPositions(newCopiedPositions);
    particleSystem->setVelocities(newCopiedVelocities);
    cpuParallelEnergyConservationTest->runTest(iterations);
    delete cpuParallelEnergyConservationTest;

    std::cout << "Energy test: GPU\n";
    originalPositions = copiedPositions;
    std::copy(originalPositions, originalPositions + particleSystem->size(), newCopiedPositions);

    originalVelocities = copiedVelocities;
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), newCopiedVelocities);

    originalAccelerations = copiedAccelerations;
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), newCopiedAccelerations);
    particleSystem->setAccelerations(newCopiedAccelerations);
    particleSystem->setPositions(newCopiedPositions);
    particleSystem->setVelocities(newCopiedVelocities);
    Test *gpuEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    gpuEnergyConservationTest->runTest(iterations);
    delete gpuEnergyConservationTest;

    particleSystem->setAccelerations(copiedAccelerations);
    particleSystem->setPositions(copiedPositions);
    particleSystem->setVelocities(copiedVelocities);
    forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
    Test *gpuOptimizedEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    std::cout << "Energy test: GPU optimized\n";
    gpuOptimizedEnergyConservationTest->runTest(iterations);
    delete gpuOptimizedEnergyConservationTest;

    delete particleSystem;
    delete[] copiedVelocities;
    delete[] copiedPositions;
    delete[] copiedAccelerations;
    delete[] newCopiedVelocities;
    delete[] newCopiedAccelerations;
    delete[] newCopiedPositions;
}