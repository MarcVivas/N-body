#include "ArgumentsParser.h"

#include "ParticleSystemCubeInitializer.h"
#include "ParticleSystemGalaxyInitializer.h"
#include "ParticleSystemLagrange.h"
#include "ParticleSystemSphere.h"
#include "ParticleSystemBall.h"
#include "ParticleSystemCubeSurface.h"
#include "ParticleSystemFile.h"


#include "ParticleSolverCPUSequential.h"
#include "ParticleSolverCPUParallel.h"
#include "ParticleSolverGPU.h"
#include "ParticleSolverCPUGrid.h"


#include "WindowInputManager.h"

int main(int argc, char *argv[])
{
    // Get the arguments
    ArgumentsParser args(argc, argv);

    // Using meters
    glm::vec3 worldDimensions(5.f, 5.f, 5.f);

    // Using pixels
    glm::vec2 windowDim(800, 600);
    Window window(windowDim, "N-body simulation");

    RenderLoop renderLoop(window, true, false);
    renderLoop.setIsBenchmark(args.isBenchmark());

    ParticleSystemInitializer* particleSystemInitializer;

    std::string filePath = args.getFilePath();

    switch (args.getInitializationType()) {
        case InitializationType::GALAXY:
            particleSystemInitializer = new ParticleSystemGalaxyInitializer(args.getNumParticles());
            break;
        case InitializationType::CUBE:
            particleSystemInitializer = new ParticleSystemCubeInitializer(args.getNumParticles());
            break;
        case InitializationType::LAGRANGE:
            particleSystemInitializer = new ParticleSystemLagrange();
            break;
        case InitializationType::SPHERE:
            particleSystemInitializer = new ParticleSystemSphere(args.getNumParticles());
            break;
        case InitializationType::BALL:
            particleSystemInitializer = new ParticleSystemBall(args.getNumParticles());
            break;
        case InitializationType::CUBE_SURFACE:
            particleSystemInitializer = new ParticleSystemCubeSurface(args.getNumParticles());
            break;
        case InitializationType::SYSTEM_FILE:
            particleSystemInitializer = new ParticleSystemFile(filePath);
            break;
        default:
            exit(EXIT_FAILURE);
    }


    ParticleSimulation* particleSimulation;

    std::string positionsCalculatorPath;
    std::string forceCalculatorPath;

    switch (args.getVersion()){
        case Version::PP_CPU_SEQUENTIAL:
            particleSimulation = new ParticleSimulation(particleSystemInitializer,  new ParticleSolverCPUSequential(args.getTimeStep(), args.getSquaredSoftening()), worldDimensions, windowDim, args.getSaveFileName());
            break;
        case Version::PP_CPU_PARALLEL:
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverCPUParallel(args.getTimeStep(), args.getSquaredSoftening()), worldDimensions, windowDim, args.getSaveFileName());
            break;
        case Version::PP_GPU_PARALLEL:
            positionsCalculatorPath = "../src/shaders/ComputeShaders/updateParticles.glsl";
            forceCalculatorPath = "../src/shaders/ComputeShaders/forceCalculation.glsl";
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverGPU(args.getTimeStep(), args.getSquaredSoftening(), positionsCalculatorPath, forceCalculatorPath), worldDimensions, windowDim, args.getSaveFileName());
            break;
        case Version::PP_GPU_OPTIMIZED:
            positionsCalculatorPath = "../src/shaders/ComputeShaders/updateParticles.glsl";
            forceCalculatorPath = "../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverGPU(320.0, args.getTimeStep(), args.getSquaredSoftening(), positionsCalculatorPath, forceCalculatorPath), worldDimensions, windowDim, args.getSaveFileName());
            break;
        case Version::GRID_CPU:
            particleSimulation = new ParticleSimulation(particleSystemInitializer, new ParticleSolverCPUGrid(new GridCPU(worldDimensions, args.getNumParticles(), 4), args.getTimeStep(), args.getSquaredSoftening()), worldDimensions, windowDim, args.getSaveFileName());
            break;
    }
    WindowInputManager windowInputManager(&window, &renderLoop, particleSimulation);

    renderLoop.runLoop(particleSimulation);

    delete particleSimulation;
    delete particleSystemInitializer;
}
