
#include "ParticleSystemInitializer.h"

#ifndef PARTICLESYSTEMCUBEINITIALIZER_H
#define PARTICLESYSTEMCUBEINITIALIZER_H


class ParticleSystemCubeInitializer: public ParticleSystemInitializer{

public:
    ParticleSystemCubeInitializer(size_t numParticles);
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;
};


#endif //PARTICLESYSTEMCUBEINITIALIZER_H
