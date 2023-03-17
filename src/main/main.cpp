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
#include "../include/Camera3D/Camera.h"
#include "../include/Shader/Shader.cpp"
#include "../include/enums/enums.h"
#include "../include/Particle/Particle.cpp"
#include "../include/ParticleSystemSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp"
#include "../include/ParticleSystemSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp"
#include "../include/ParticleSystemInitializer/ParticleSystemCubeInitializer/ParticleSystemCubeInitializer.cpp"
#include "../include/ParticleSystemInitializer/ParticleSystemGalaxyInitializer/ParticleSystemGalaxyInitializer.cpp"
#include "../include/ParticleSystemInitializer/interface/ParticleSystemInitializer.h"
#include "../include/ParticleSystem/AbstractClass/ParticleSystem.cpp"
#include "../include/OpenGLRenderer/OpenGLRenderer.cpp"
#include "../include/ParticleSystem/ParticleSystemCPU/ParticleSystemCPU.cpp"
#include "../include/ParticleSystem/ParticleSystemGPU/ParticleSystemGPU.cpp"
#include "../include/ArgumentsParser/ArgumentsParser.cpp"


int main(int argc, char *argv[])
{
    // Get the arguments
    ArgumentsParser args(argc, argv);

    // In meters
    glm::vec3 worldDimensions(44458.f, 44458.f, 44458.f);

    OpenGLRenderer renderer(new Camera(600, 600, worldDimensions), "N-body simulation", false, true);


    ParticleSystem* particleSystem;
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



    switch (args.getVersion()){
        case Version::PP_CPU_SEQUENTIAL:
            particleSystem = new ParticleSystemCPU(particleSystemInitializer,  new ParticleSolverCPUSequential(), worldDimensions);
            break;
        case Version::PP_CPU_PARALLEL:
            particleSystem = new ParticleSystemCPU(particleSystemInitializer, new ParticleSolverCPUParallel(), worldDimensions);
            break;
        case Version::PP_GPU_PARALLEL:
            //particleSystem = new ParticleSystemGPU(args.getNumParticles(), args.getInitializationType(), worldDimensions);
            break;
        default:
            exit(EXIT_FAILURE);
    }

    //std::cout << *particleSystem << std::endl;


    renderer.render_loop(particleSystem);

    delete particleSystem;
    delete particleSystemInitializer;




}
