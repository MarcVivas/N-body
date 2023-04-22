
#include "ParticleSystemInitializer.h"

#ifndef N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H
#define N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H


class ParticleSystemGalaxyInitializer: public ParticleSystemInitializer{
public:
    ParticleSystemGalaxyInitializer(size_t numParticles);
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;

};


#endif //N_BODY_PARTICLESYSTEMGALAXYINITIALIZER_H
