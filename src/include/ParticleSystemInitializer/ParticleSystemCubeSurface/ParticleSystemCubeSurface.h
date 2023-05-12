
#include "ParticleSystemInitializer.h"

#ifndef PARTICLESYSTEMCUBESURFACE_H
#define PARTICLESYSTEMCUBESURFACE_H


class ParticleSystemCubeSurface: public ParticleSystemInitializer{

public:
    ParticleSystemCubeSurface(size_t numParticles);
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;
};


#endif //PARTICLESYSTEMCUBESURFACE_H
