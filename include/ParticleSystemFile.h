
#ifndef N_BODY_PARTICLESYSTEMFILE_H
#define N_BODY_PARTICLESYSTEMFILE_H

#include "ParticleSystemInitializer.h"


class ParticleSystemFile: public ParticleSystemInitializer{

public:
    ParticleSystemFile(std::string filePath);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions, const bool usesGPU);

private:
    std::string path;
};


#endif //N_BODY_PARTICLESYSTEMFILE_H
