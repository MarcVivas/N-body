

#ifndef PARTICLESYSTEMCUBEINITIALIZER_H
#define PARTICLESYSTEMCUBEINITIALIZER_H

#include "ParticleSystemInitializer.h"


class ParticleSystemCubeInitializer: public ParticleSystemInitializer{

public:
    ParticleSystemCubeInitializer(size_t numParticles);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions, const bool usesGPU);

private:
    size_t totalParticles;
};


#endif //PARTICLESYSTEMCUBEINITIALIZER_H
