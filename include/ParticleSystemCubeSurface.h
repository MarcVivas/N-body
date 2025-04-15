

#ifndef PARTICLESYSTEMCUBESURFACE_H
#define PARTICLESYSTEMCUBESURFACE_H
#include "ParticleSystemInitializer.h"


class ParticleSystemCubeSurface: public ParticleSystemInitializer{

public:
    ParticleSystemCubeSurface(size_t numParticles);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;
};


#endif //PARTICLESYSTEMCUBESURFACE_H
