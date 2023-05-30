#include <iostream>
#include <ctime>
#include <glm/glm.hpp>
#include "ArgumentsParser.h"

#include "CPUEnergyConservationTest.h"
#include "GPUEnergyConservationTest.h"
#include "DeterministicCPUSolverTest.h"
#include "DeterministicGPUSolverTest.h"

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

    const size_t iterations = 150000;


    std::string positionsShaderPath = "../../src/shaders/ComputeShaders/updateParticles.glsl";
    std::string forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculation.glsl";



    std::cout << "Energy test: CPU sequential\n";
    ParticleSystem *copy = new ParticleSystem(particleSystem);
    Test *cpuSeqEnergyConservationTest = new CPUEnergyConservationTest(copy, new ParticleSolverCPUSequential(args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());
    cpuSeqEnergyConservationTest->runTest(iterations);
    delete copy;
    delete cpuSeqEnergyConservationTest;

    std::cout << "Energy test: CPU parallel\n";
    copy = new ParticleSystem(particleSystem);
    Test *cpuParallelEnergyConservationTest = new CPUEnergyConservationTest(copy, new ParticleSolverCPUParallel(args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());
    cpuParallelEnergyConservationTest->runTest(iterations);
    delete cpuParallelEnergyConservationTest;
    delete copy;

    std::cout << "Energy test: GPU\n";
    copy = new ParticleSystem(particleSystem);
    Test *gpuEnergyConservationTest = new GPUEnergyConservationTest(copy, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    gpuEnergyConservationTest->runTest(iterations);
    delete gpuEnergyConservationTest;
    delete copy;


    forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
    copy = new ParticleSystem(particleSystem);
    Test *gpuOptimizedEnergyConservationTest = new GPUEnergyConservationTest(copy, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    std::cout << "Energy test: GPU optimized\n";
    gpuOptimizedEnergyConservationTest->runTest(iterations);
    delete gpuOptimizedEnergyConservationTest;
    delete copy;

    copy = new ParticleSystem(particleSystem);
    Test *cpuParallelGridEnergyConservationTest = new CPUEnergyConservationTest(particleSystem, new ParticleSolverCPUGrid(new GridCPU(glm::vec3(5, 5, 5), particleSystem->size(), 4), args.getTimeStep(), args.getSquaredSoftening()), args.getTimeStep(), args.getSquaredSoftening());
    std::cout << "Energy test: CPU parallel grid\n";
    cpuParallelGridEnergyConservationTest->runTest(iterations);
    delete cpuParallelGridEnergyConservationTest;
    delete copy;

    std::cout << "Version 1 - Deterministic test\n";
    copy = new ParticleSystem(particleSystem);
    Test * version = new DeterministicCPUSolverTest(copy, new ParticleSolverCPUSequential(args.getTimeStep(), args.getSquaredSoftening()));
    version->runTest(iterations);
    delete version;

    std::cout << "Version 2 - Deterministic test\n";
    copy = new ParticleSystem(particleSystem);
    version = new DeterministicCPUSolverTest(copy, new ParticleSolverCPUParallel(args.getTimeStep(), args.getSquaredSoftening()));
    version->runTest(iterations);
    delete version;

    std::cout << "Version 3 - Deterministic test\n";
    copy = new ParticleSystem(particleSystem);
    forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculation.glsl";
    version = new DeterministicGPUSolverTest(copy, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    version->runTest(iterations);
    delete version;

    std::cout << "Version 4 - Deterministic test\n";
    copy = new ParticleSystem(particleSystem);
    forcesShaderPath = "../../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
    version = new DeterministicGPUSolverTest(copy, positionsShaderPath, forcesShaderPath, args.getTimeStep(), args.getSquaredSoftening());
    version->runTest(iterations);
    delete version;


    delete particleSystem;

}