//
// Created by marc on 5/03/23.
//

#include "ArgumentsParser.h"

ArgumentsParser::ArgumentsParser(int argc, char *argv[]) {
    // Default values
    this->version = PP_CPU_SEQUENTIAL;
    this->numParticles = 100;
    this->init = GALAXY;
    this->test = false;
    this->benchmark = false;

    std::cout << "============================================ \n\n";
    std::cout << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init]\n";
    std::cout << "Default: " << argv[0] << " -v 1 -n 100 -i 2\n\n";

    std::cout << "Available versions: \n";
    std::cout << "-v 1 (Particle-Particle algorithm CPU sequential)\n";
    std::cout << "-v 2 (Particle-Particle algorithm CPU parallel)\n";
    std::cout << "-v 3 (Particle-Particle algorithm GPU parallel)\n\n";

    std::cout << "Number of particles: \n";
    std::cout << "-n (Any positive number)\n\n";

    std::cout << "Available initializations: \n";
    std::cout << "-i 1 (Particles form a CUBE, have random velocities and masses) \n";
    std::cout << "-i 2 (Particles form a GALAXY, have random velocities and masses) \n\n";

    std::cout << "============================================ \n\n";

    int option;
    while ((option = getopt(argc, argv, "v:n:i:bt")) != -1) {
        switch (option) {
            case 'v':
                version = static_cast<Version>(atoi(optarg));
                if (version < static_cast<int>(Version::PP_CPU_SEQUENTIAL) || version > static_cast<int>(Version::PP_GPU_PARALLEL)) {
                    std::cerr << "Invalid version\n";
                    exit(EXIT_FAILURE);
                }
                break;
            case 'n':
                numParticles = atoi(optarg);
                break;
            case 'i':
                init = static_cast<InitializationType>(atoi(optarg));
                if (init < static_cast<int>(InitializationType::CUBE) || init > static_cast<int>(InitializationType::GALAXY)) {
                    std::cerr << "Invalid initialization type\n";
                    exit(EXIT_FAILURE);
                }
                break;
            case 'b':
                this->benchmark = true;
                break;
            case 't':
                this->test = true;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init]\n";
                exit(EXIT_FAILURE);
        }
    }


    std::cout << "------------------------------------ \n\n";
    std::cout << "Now using: \n\n";
    std::cout << "Version: " << version << "\n";
    std::cout << "Num particles: " << numParticles << "\n";
    std::cout << "Init: " << init << "\n\n";
    std::cout << "------------------------------------ \n\n";

}

Version ArgumentsParser::getVersion() {
    return this->version;
}

InitializationType ArgumentsParser::getInitializationType() {
    return this->init;
}

size_t ArgumentsParser::getNumParticles() {
    return this->numParticles;
}

bool ArgumentsParser::isTest() {
    return this->test;
}

bool ArgumentsParser::isBenchmark(){
    return this->benchmark;
}