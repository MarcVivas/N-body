#include "ParticleSystemSaver.h"
#include <fstream>
#include <string>

ParticleSystemSaver::ParticleSystemSaver(const std::string &customFileName) {
    this->fileName = customFileName;
}

void ParticleSystemSaver::saveCurrentState(int iteration, ParticleSystem* particleSystem) {
    this->saveSimulationFile(particleSystem, "current", iteration);
}

void ParticleSystemSaver::saveFinalState(ParticleSystem* particleSystem) {
    this->saveSimulationFile(particleSystem, "final", 0);
}

void ParticleSystemSaver::saveInitialState(ParticleSystem *particleSystem) {
    this->saveSimulationFile(particleSystem, "initial", 0);
}

void ParticleSystemSaver::saveSimulationFile(ParticleSystem *particleSys, const std::string &fileType, int iteration) {
    if(fileName.empty()) return;

    static int saveCount = 1;
    std::string finalFileName;

    finalFileName = fileName + std::to_string(saveCount);

    if (fileType == "initial") {
        finalFileName += "_ini.sim";
    } else if (fileType == "current") {
        finalFileName += "_iter" + std::to_string(iteration) + ".sim";
    } else if (fileType == "final") {
        finalFileName += "_end.sim";
    } else {
        std::cout << "Invalid file type specified." << std::endl;
        return;
    }

    // Check if the file already exists
    std::ifstream file(finalFileName);
    while (file.good()) {
        file.close();
        saveCount++;
        finalFileName = fileName + std::to_string(saveCount);

        if (fileType == "initial") {
            finalFileName += "_ini.sim";
        } else if (fileType == "current") {
            finalFileName += "_iter" + std::to_string(iteration) + ".sim";
        } else if (fileType == "final") {
            finalFileName += "_end.sim";
        }
        file.open(finalFileName);
    }
    file.close();

    // Open the file for writing
    std::ofstream outFile(finalFileName);
    if (!outFile.is_open()) {
        std::cout << "Failed to open the save file." << std::endl;
        return;
    }

    // Write the ParticleSystem data to the file
    outFile << *particleSys;

    // Close the file
    outFile.close();

    std::cout << "Particle system saved to " << finalFileName << std::endl;
}