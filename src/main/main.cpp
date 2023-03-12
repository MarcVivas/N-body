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
#include <glm/gtc/random.hpp>
#include "../include/Shader/Shader.cpp"
#include "../include/enums/enums.h"
#include "../include/Particle/Particle.cpp"
#include "../include/ParticleSystemSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp"
#include "../include/ParticleSystemSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp"
#include "../include/ParticleSystemInitializer/ParticleSystemCubeInitializer/ParticleSystemCubeInitializer.cpp"
#include "../include/ParticleSystem/AbstractClass/ParticleSystem.cpp"
#include "../include/OpenGLRenderer/OpenGLRenderer.cpp"
#include "../include/ParticleSystem/ParticleSystemCPU/ParticleSystemCPU.cpp"
#include "../include/ParticleSystem/ParticleSystemGPU/ParticleSystemGPU.cpp"
#include "../include/ArgumentsParser/ArgumentsParser.cpp"


int main(int argc, char *argv[])
{
    // Get the arguments
    ArgumentsParser args(argc, argv);

    OpenGLRenderer renderer(600, 600, "N-body simulation", false, true);


    ParticleSystem* particleSystem;
    switch (args.getVersion()){
        case Version::PP_CPU_SEQUENTIAL:
            particleSystem = new ParticleSystemCPU(new ParticleSystemCubeInitializer(args.getNumParticles()),  new ParticleSolverCPUSequential());
            break;
        case Version::PP_CPU_PARALLEL:
            particleSystem = new ParticleSystemCPU(new ParticleSystemCubeInitializer(args.getNumParticles()), new ParticleSolverCPUParallel());
            break;
        case Version::PP_GPU_PARALLEL:
            //particleSystem = new ParticleSystemGPU(args.getNumParticles(), args.getInitializationType());
            break;
        default:
            exit(EXIT_FAILURE);
    }

    //std::cout << *particleSystem << std::endl;


    renderer.render_loop(particleSystem);

    delete particleSystem;




}
