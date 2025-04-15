
#ifndef N_BODY_PARTICLESYSTEMSPHERE_H
#define N_BODY_PARTICLESYSTEMSPHERE_H

#include "ParticleSystemInitializer.h"

class ParticleSystemSphere: public ParticleSystemInitializer{

public:
    ParticleSystemSphere(size_t numParticles);
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;
};


#endif //N_BODY_PARTICLESYSTEMSPHERE_H
