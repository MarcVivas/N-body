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
    ParticleSystemInitializer *initializer = new ParticleSystemCubeInitializer(2 + (rand() % 5));
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

    std::string shaderPath = "../../src/shaders/ComputeShaders/updateParticles.glsl";
    Test *gpuEnergyConservationTest = new GPUEnergyConservationTest(particleSystem, shaderPath, args.getTimeStep(), args.getSquaredSoftening());



    std::cout << "Energy test: CPU sequential\n";
    cpuSeqEnergyConservationTest->runTest(iterations);

    std::cout << "Energy test: CPU parallel\n";
    particleSystem->setAccelerations(copiedAccelerations);
    particleSystem->setPositions(copiedPositions);
    particleSystem->setVelocities(copiedVelocities);
    cpuParallelEnergyConservationTest->runTest(iterations);

    std::cout << "Energy test: GPU\n";
    gpuEnergyConservationTest->runTest(iterations);

    delete cpuSeqEnergyConservationTest;
    delete cpuParallelEnergyConservationTest;
    delete gpuEnergyConservationTest;
    delete particleSystem;
    delete[] copiedVelocities;
    delete[] copiedPositions;
    delete[] copiedAccelerations;
}