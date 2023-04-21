#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <sstream>
#include <fstream>
#include <omp.h>

#include "../src/include/Shader/abstract/Shader.cpp"
#include "../src/include/Shader/ComputeShader/ComputeShader.cpp"
#include "../src/include/Particle/Particle.cpp"
#include "tests/EnergyConservationTest/CPUEnergyConservationTest.cpp"
#include "tests/EnergyConservationTest/GPUEnergyConservationTest.cpp"
#include "../src/include/ParticleSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp"
#include "../src/include/ParticleSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp"

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