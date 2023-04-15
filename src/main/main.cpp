#include "../../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <omp.h>
#include <cstring>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../include/Camera/Camera.cpp"

#include "../include/Shader/abstract/Shader.cpp"
#include "../include/Shader/ComputeShader/ComputeShader.cpp"
#include "../include/Shader/VertexFragmentShader/VertexFragmentShader.cpp"

#include "../include/enums/enums.h"
#include "../include/Particle/Particle.cpp"
#include "../include/ParticleSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp"
#include "../include/ParticleSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp"
#include "../include/ParticleSolver/ParticleSolverGPU/ParticleSolverGPU.cpp"
#include "../include/ParticleSystemInitializer/ParticleSystemCubeInitializer/ParticleSystemCubeInitializer.cpp"
#include "../include/ParticleSystemInitializer/ParticleSystemGalaxyInitializer/ParticleSystemGalaxyInitializer.cpp"
#include "../include/ParticleSystemInitializer/interface/ParticleSystemInitializer.h"
#include "../include/ArgumentsParser/ArgumentsParser.cpp"

#include "../include/Bloom/Bloom.cpp"
#include "../include/Window/Window.cpp"
#include "../include/RenderTimer/RenderTimer.cpp"
#include "../include/ParticleDrawer/ParticleDrawer.cpp"
#include "../include/ParticleSimulation/ParticleSimulation.cpp"
#include "../include/RenderLoop/RenderLoop.cpp"
#include "../include/WindowInputManager/WindowInputManager.cpp"

int main(int argc, char *argv[])
{
    // Get the arguments
    ArgumentsParser args(argc, argv);


    // Using meters
    glm::vec3 worldDimensions(8000.f, 8000.f, 8000.f);

    // Using pixels
    glm::vec2 windowDim(800, 600);
    Window window(windowDim, "N-body simulation");

    RenderLoop renderLoop(window, true, false);
    renderLoop.setIsBenchmark(args.isBenchmark());

    ParticleSystemInitializer* particleSystemInitializer;

    switch (args.getInitializationType()) {
        case InitializationType::GALAXY:
            particleSystemInitializer = new ParticleSystemGalaxyInitializer(args.getNumParticles());
            break;
        case InitializationType::CUBE:
            particleSystemInitializer = new ParticleSystemCubeInitializer(args.getNumParticles());
            break;
        default:
            exit(EXIT_FAILURE);
    }


    ParticleSimulation* particleSimulation;

    switch (args.getVersion()){
        case Version::PP_CPU_SEQUENTIAL:
            particleSimulation = new ParticleSimulation(particleSystemInitializer,  new ParticleSolverCPUSequential(), worldDimensions, windowDim);
            break;
        case Version::PP_CPU_PARALLEL:
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverCPUParallel(), worldDimensions, windowDim);
            break;
        case Version::PP_GPU_PARALLEL:
            std::string computeShader("../src/shaders/ComputeShaders/updateParticles.glsl");
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverGPU(computeShader), worldDimensions, windowDim);
            break;
    }


    WindowInputManager windowInputManager(&window, &renderLoop, particleSimulation);

    renderLoop.runLoop(particleSimulation);

    //std::cout << *particleSystem << std::endl;

    delete particleSimulation;
    delete particleSystemInitializer;
}
