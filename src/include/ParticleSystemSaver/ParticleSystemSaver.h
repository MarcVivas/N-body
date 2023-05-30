#include <iostream>
#include "ParticleSystem.h"

#ifndef N_BODY_PARTICLESYSTEMSAVER_H
#define N_BODY_PARTICLESYSTEMSAVER_H


class ParticleSystemSaver {
public:
    ParticleSystemSaver(const std::string& customFileName);
    void saveInitialState(ParticleSystem* particleSystem);
    void saveCurrentState(int iteration, ParticleSystem* particleSystem);
    void saveFinalState(ParticleSystem* particleSystem);
protected:
    void saveSimulationFile(ParticleSystem* particleSys, const std::string& fileType, int iteration);
    std::string fileName;


};


#endif //N_BODY_PARTICLESYSTEMSAVER_H
