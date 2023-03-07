#include "../../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <getopt.h>
#include <vector>
#include "../include/enums/enums.h"
#include "../include/Particle/Particle.cpp"
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
            particleSystem = new ParticleSystemCPU(args.getNumParticles(), args.getInitializationType());
            break;
        case Version::PP_CPU_PARALLEL:
            std::cerr << "Not yet implemented \n";
            exit(EXIT_FAILURE);

        case Version::PP_GPU_PARALLEL:
            //particleSystem = new ParticleSystemGPU(args.getNumParticles(), args.getInitializationType());
            break;
        default:
            exit(EXIT_FAILURE);
    }
    std::cout << *particleSystem;


    renderer.render_loop(particleSystem);

    delete particleSystem;




}
