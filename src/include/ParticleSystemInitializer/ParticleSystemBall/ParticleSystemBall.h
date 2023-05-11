#include "ParticleSystemInitializer.h"

#ifndef N_BODY_PARTICLESYSTEMBALL_H
#define N_BODY_PARTICLESYSTEMBALL_H


class ParticleSystemBall: public ParticleSystemInitializer{

public:
    ParticleSystemBall(size_t numParticles);
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;
};


#endif //N_BODY_PARTICLESYSTEMBALL_H
