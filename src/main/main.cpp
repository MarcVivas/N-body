#include "ArgumentsParser.h"

#include "ParticleSystemCubeInitializer.h"
#include "ParticleSystemGalaxyInitializer.h"

#include "ParticleSolverCPUSequential.h"
#include "ParticleSolverCPUParallel.h"
#include "ParticleSolverGPU.h"

#include "WindowInputManager.h"

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

    delete particleSimulation;
    delete particleSystemInitializer;
}
