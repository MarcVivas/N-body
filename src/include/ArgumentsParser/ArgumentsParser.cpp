//
// Created by marc on 5/03/23.
//

#include "ArgumentsParser.h"

ArgumentsParser::ArgumentsParser(int argc, char *argv[]) {
    // Default values
    this->version = PP_CPU_SEQUENTIAL;
    this->numParticles = 100;
    this->init = GALAXY;

    if(argc == 1){
        std::cout << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init]\n";
    }

    int option;
    while ((option = getopt(argc, argv, "v:n:i:")) != -1) {
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
            default:
                std::cerr << "Usage: " << argv[0] << " [-v version] [-n numParticles] [-i init]\n";
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Version: " << version << "\n";
    std::cout << "Num particles: " << numParticles << "\n";
    std::cout << "Init: " << init << "\n";
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