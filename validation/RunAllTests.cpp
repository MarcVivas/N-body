#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include "ArgumentsParser.h"

#include "CPUEnergyConservationTest.h"
#include "GPUEnergyConservationTest.h"

#include "ParticleSystemCubeInitializer.h"
#include "ParticleSolverCPUParallel.h"
#include "ParticleSolverCPUSequential.h"
#include "ParticleSolverCPUGrid.h"


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

    const size_t iterations = 3000;

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
    glm::vec4* newCopiedPositions2 = new glm::vec4[particleSystem->size()];
    std::copy(originalPositions, originalPositions + particleSystem->size(), newCopiedPositions2);

    originalVelocities = copiedVelocities;
    glm::vec4* newCopiedVelocities2 = new glm::vec4[particleSystem->size()];
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), newCopiedVelocities2);

    originalAccelerations = copiedAccelerations;
    glm::vec4* newCopiedAccelerations2 = new glm::vec4[particleSystem->size()];
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), newCopiedAccelerations2);
    particleSystem->setAccelerations(newCopiedAccelerations2);
    particleSystem->setPositions(newCopiedPositions2);
    particleSystem->setVelocities(newCopiedVelocities2);
    Test *gpuEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    gpuEnergyConservationTest->runTest(iterations);
    delete gpuEnergyConservationTest;

    originalPositions = copiedPositions;
    glm::vec4* newCopiedPositions3 = new glm::vec4[particleSystem->size()];
    std::copy(originalPositions, originalPositions + particleSystem->size(), newCopiedPositions3);

    originalVelocities = copiedVelocities;
    glm::vec4* newCopiedVelocities3 = new glm::vec4[particleSystem->size()];
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), newCopiedVelocities3);

    originalAccelerations = copiedAccelerations;
    glm::vec4* newCopiedAccelerations3 = new glm::vec4[particleSystem->size()];
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), newCopiedAccelerations3);
    particleSystem->setAccelerations(newCopiedAccelerations3);
    particleSystem->setPositions(newCopiedPositions3);
    particleSystem->setVelocities(newCopiedVelocities3);
    forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
    Test *gpuOptimizedEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    std::cout << "Energy test: GPU optimized\n";
    gpuOptimizedEnergyConservationTest->runTest(iterations);
    delete gpuOptimizedEnergyConservationTest;

    originalPositions = copiedPositions;
    glm::vec4* newCopiedPositions4 = new glm::vec4[particleSystem->size()];
    std::copy(originalPositions, originalPositions + particleSystem->size(), newCopiedPositions4);

    originalVelocities = copiedVelocities;
    glm::vec4* newCopiedVelocities4 = new glm::vec4[particleSystem->size()];
    std::copy(originalVelocities, originalVelocities + particleSystem->size(), newCopiedVelocities4);

    originalAccelerations = copiedAccelerations;
    glm::vec4* newCopiedAccelerations4 = new glm::vec4[particleSystem->size()];
    std::copy(originalAccelerations, originalAccelerations + particleSystem->size(), newCopiedAccelerations4);
    particleSystem->setAccelerations(newCopiedAccelerations4);
    particleSystem->setPositions(newCopiedPositions4);
    particleSystem->setVelocities(newCopiedVelocities4);
    Test *cpuParallelGridEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUGrid(new GridCPU(glm::vec3(5, 5, 5), particleSystem->size(), 4), args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());
    std::cout << "Energy test: CPU parallel grid\n";
    cpuParallelGridEnergyConservationTest->runTest(iterations);
    delete cpuParallelGridEnergyConservationTest;

    delete particleSystem;
    delete[] copiedVelocities;
    delete[] copiedPositions;
    delete[] copiedAccelerations;
    delete[] newCopiedVelocities4;
    delete[] newCopiedAccelerations4;
    delete[] newCopiedPositions4;

}