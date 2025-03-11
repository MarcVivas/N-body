#include "ArgumentsParser.h"
#include <iostream>

ArgumentsParser::ArgumentsParser(int argc, char *argv[]) {
    // Default values
    this->version = Version::PP_CPU_SEQUENTIAL;
    this->numParticles = 100;
    this->init = InitializationType::GALAXY;
    this->timeStep = .001f;
    this->squaredSoftening = .0935f;
    this->benchmark = false;

    std::cout << "============================================ \n\n";
    std::cout << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init] [-t timeStep] [-s squaredSoftening] [-f filePath]\n";
    std::cout << "Alternative usage: " << argv[0] << " [-version version] [-n numParticles] [-init init] [-time-step timeStep] [-softening squaredSoftening] [-file filePath] \n";
    std::cout << "Default: " << argv[0] << " -v 1 -n 100 -i 2 -t 0.00004\n\n";

    std::cout << "Available versions: \n";
    std::cout << "-v 1 (Particle-Particle algorithm CPU sequential)\n";
    std::cout << "-v 2 (Particle-Particle algorithm CPU parallel)\n";
    std::cout << "-v 3 (Particle-Particle algorithm GPU parallel)\n";
    std::cout << "-v 4 (Optimized version 3. GPU parallel Fast N-body Particle-Particle)\n";
    std::cout << "-v 5 Fixed grid CPU\n\n";

    std::cout << "Number of particles: \n";
    std::cout << "-n (Any positive number)\n\n";

    std::cout << "Available initializations: \n";
    std::cout << "-i 1 (Particles form a CUBE) \n";
    std::cout << "-i 2 (Particles form a DISK) \n";
    std::cout << "-i 3 (Particles form a equilateral TRIANGLE, have equal masses and 0 velocity)\n";
    std::cout << "-i 4 (Particles form a SPHERE (Only the surface))\n";
    std::cout << "-i 5 (Particles form a BALL)\n";
    std::cout << "-i 6 (Particles form a CUBE (Only the surface))\n\n";

    std::cout << "File initialization: \n";
    std::cout << "-f path/to/initialization/file \n\n";

    std::cout << "Name of simulation saves: \n";
    std::cout << "-o customFileName \n\n";

    std::cout << "Time step: \n";
    std::cout << "-t (Any positive decimal number)\n\n";

    std::cout << "Squared softening: \n";
    std::cout << "-s (Any positive decimal number)\n\n";

    std::cout << "============================================ \n\n";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == "-version" || arg == "-v") && i + 1 < argc) {
            int value = std::stoi(argv[i + 1]);
            if (value >= static_cast<int>(Version::PP_CPU_SEQUENTIAL) &&
                value <= static_cast<int>(Version::BARNES_HUT_GPU_PARALLEL)) {
                this->version = static_cast<Version>(value);
            } else {
                std::cerr << "Invalid version\n";
                exit(EXIT_FAILURE);
            }
            i++;
        } else if (arg == "-n" && i + 1 < argc) {
            int value = std::stoi(argv[i + 1]);
            if (value > 0) {
                this->numParticles = value;
            } else {
                std::cerr << "Invalid number of particles\n";
                exit(EXIT_FAILURE);
            }
            i++;
        } else if ((arg == "-init" || arg == "-i") && i + 1 < argc) {
            int value = std::stoi(argv[i + 1]);
            if (value >= static_cast<int>(InitializationType::CUBE) &&
                value <= static_cast<int>(InitializationType::CUBE_SURFACE)) {
                this->init = static_cast<InitializationType>(value);
            } else {
                std::cerr << "Invalid initialization type\n";
                exit(EXIT_FAILURE);
            }
            i++;
        }
        else if (arg == "-b") {
            this->benchmark = true;
        }
        else if ((arg == "-time-step" || arg == "-t") && i + 1 < argc) {
            this->timeStep = std::stof(argv[i+1]);
            i++;
        }
        else if ((arg == "-softening" || arg == "-s") && i + 1 < argc) {
            this->squaredSoftening = std::stof(argv[i+1]);
            i++;
        }
        else if ((arg == "-file" || arg == "-f") && i + 1 < argc) {
            filePath = argv[i + 1];
            i++;
            this->init = static_cast<InitializationType>(7);
            std::cout << "------------------------------------ \n\n";
            std::cout << "Now using: \n\n";
            std::cout << "File initialization\n";
            std::cout << "------------------------------------ \n\n";
            return;
        }
        else if ((arg == "-o" || arg == "-output") && i + 1 < argc) {
            saveFileName = argv[i + 1];
            i++;
        }
        else {
            std::cerr << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init] [-t timeStep] [-s squaredSoftening] [-f filePath]\n";
            std::cerr << "Alternative usage: " << argv[0] << " [-version version] [-n numParticles] [-init init] [-time-step timeStep] [-softening squaredSoftening] [-file filePath] \n";
            exit(EXIT_FAILURE);
        }
    }

    if(filePath.empty()){
        std::cout << "------------------------------------ \n\n";
        std::cout << "Now using: \n\n";
        std::cout << "Version: " << version << "\n";
        std::cout << "Num particles: " << numParticles << "\n";
        std::cout << "Init: " << init << '\n';
        std::cout << "Time step: " << timeStep << '\n';
        std::cout << "Squared softening: " << squaredSoftening << "\n\n";
        std::cout << "------------------------------------ \n\n";
    }
}

Version ArgumentsParser::getVersion() {
    return this->version;
}

std::string ArgumentsParser::getSaveFileName() {
    return saveFileName;
}

InitializationType ArgumentsParser::getInitializationType() {
    return this->init;
}

size_t ArgumentsParser::getNumParticles() {
    return this->numParticles;
}

float ArgumentsParser::getTimeStep() {
    return this->timeStep;
}

float ArgumentsParser::getSquaredSoftening() {
    return this->squaredSoftening;
}

bool ArgumentsParser::isBenchmark(){
    return this->benchmark;
}

std::string ArgumentsParser::getFilePath() {
    return this->filePath;
}