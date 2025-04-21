

#ifndef N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H
#define N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H
#include "ParticleSystemInitializer.h"


class ParticleSystemGalaxyInitializer: public ParticleSystemInitializer{
public:
    ParticleSystemGalaxyInitializer(size_t numParticles);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions, const bool usesGPU);

private:
    size_t totalParticles;

};


#endif //N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H
