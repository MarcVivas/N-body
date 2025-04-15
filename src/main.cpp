#include <memory>
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
#include "ParticleSolverBHutCPUSeq.h"
#include "ParticleSolverBHutCPUParallel.h"
#include "ParticleSolverBHutGPU.h"



#include "WindowInputManager.h"

int main(int argc, char *argv[])
{
    // Get the arguments
    ArgumentsParser args(argc, argv);

    glm::vec3 worldDimensions(5.f);

    // Using pixels
    glm::vec2 windowDim(1300, 750);
    Window window(windowDim, "N-body simulation");

    RenderLoop renderLoop(window, true, false);

    std::unique_ptr<ParticleSystemInitializer> particleSystemInitializer;

    std::string filePath = args.getFilePath();

    switch (args.getInitializationType()) {
        case InitializationType::GALAXY:
            particleSystemInitializer = std::make_unique<ParticleSystemGalaxyInitializer>(args.getNumParticles());
            break;
        case InitializationType::CUBE:
            particleSystemInitializer = std::make_unique<ParticleSystemCubeInitializer>(args.getNumParticles());
            break;
        case InitializationType::LAGRANGE:
            particleSystemInitializer = std::make_unique<ParticleSystemLagrange>();
            break;
        case InitializationType::SPHERE:
            particleSystemInitializer = std::make_unique<ParticleSystemSphere>(args.getNumParticles());
            break;
        case InitializationType::BALL:
            particleSystemInitializer = std::make_unique<ParticleSystemBall>(args.getNumParticles());
            break;
        case InitializationType::CUBE_SURFACE:
            particleSystemInitializer = std::make_unique<ParticleSystemCubeSurface>(args.getNumParticles());
            break;
        case InitializationType::SYSTEM_FILE:
            particleSystemInitializer = std::make_unique<ParticleSystemFile>(filePath);
            break;
        default:
            exit(EXIT_FAILURE);
    }

    std::shared_ptr<ParticleSimulation> particleSimulation;
    

    std::string positionsCalculatorPath;
    std::string forceCalculatorPath;

    switch (args.getVersion()){
        case Version::PP_CPU_SEQUENTIAL:
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverCPUSequential>(args.getTimeStep(), args.getSquaredSoftening())
                ),
                worldDimensions,
                windowDim
            );
            break;
        case Version::PP_CPU_PARALLEL:
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverCPUParallel>(args.getTimeStep(), args.getSquaredSoftening())
                ),
                worldDimensions,
                windowDim
            );
            break;
        case Version::PP_GPU_PARALLEL:
            positionsCalculatorPath = "../src/shaders/ComputeShaders/updateParticles.glsl";
            forceCalculatorPath = "../src/shaders/ComputeShaders/forceCalculation.glsl";
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverGPU>(args.getTimeStep(), args.getSquaredSoftening(), positionsCalculatorPath, forceCalculatorPath)
                ),
                worldDimensions,
                windowDim
            );
            break;
        case Version::PP_GPU_OPTIMIZED:
            positionsCalculatorPath = "../src/shaders/ComputeShaders/updateParticles.glsl";
            forceCalculatorPath = "../src/shaders/ComputeShaders/forceCalculationOptimized.glsl";
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverGPU>(320.0, args.getTimeStep(), args.getSquaredSoftening(), positionsCalculatorPath, forceCalculatorPath)
                ),
                worldDimensions,
                windowDim
            );
            break;
       
        case Version::BARNES_HUT_CPU_SEQ:
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverBHutCPUSeq>(args.getTimeStep(), args.getSquaredSoftening(), args.getNumParticles())
                ),
                worldDimensions,
                windowDim
            );
            break;
        
        case Version::BARNES_HUT_CPU_PARALLEL:
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverBHutCPUParallel>(args.getTimeStep(), args.getSquaredSoftening(), args.getNumParticles())
                    ),
                worldDimensions, 
                windowDim
            );
            break;
       
        case Version::BARNES_HUT_GPU_PARALLEL:
            positionsCalculatorPath = "../src/shaders/ComputeShaders/updateParticles.glsl";
            forceCalculatorPath = "../src/shaders/ComputeShaders/forceCalcuBarnesHut.glsl";
            particleSimulation = std::make_shared<ParticleSimulation>(
                std::move(particleSystemInitializer),
                std::move(
                    std::make_unique<ParticleSolverBHutGPU>(args.getTimeStep(), args.getSquaredSoftening(), args.getNumParticles(), positionsCalculatorPath, forceCalculatorPath)
                ),
                worldDimensions,
                windowDim
            );
            break;
    }
    WindowInputManager windowInputManager(&window, &renderLoop, particleSimulation);

    renderLoop.runLoop(particleSimulation);

}
