
#ifndef N_BODY_PARTICLESYSTEMBALL_H
#define N_BODY_PARTICLESYSTEMBALL_H

#include "ParticleSystemInitializer.h"

class ParticleSystemBall: public ParticleSystemInitializer{

public:
    ParticleSystemBall(size_t numParticles);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions, const bool usesGPU);

private:
    size_t totalParticles;
};


#endif //N_BODY_PARTICLESYSTEMBALL_H
