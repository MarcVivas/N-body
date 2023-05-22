#include "ParticleSystemInitializer.h"

#ifndef N_BODY_PARTICLESYSTEMFILE_H
#define N_BODY_PARTICLESYSTEMFILE_H


class ParticleSystemFile: public ParticleSystemInitializer{

public:
    ParticleSystemFile(std::string filePath);
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    std::string path;
};


#endif //N_BODY_PARTICLESYSTEMFILE_H
