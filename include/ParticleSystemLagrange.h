
#ifndef N_BODY_PARTICLESYSTEMLAGRANGE_H
#define N_BODY_PARTICLESYSTEMLAGRANGE_H

#include "ParticleSystemInitializer.h"

class ParticleSystemLagrange: public ParticleSystemInitializer {
public:
    ParticleSystemLagrange();
    std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;

};


#endif //N_BODY_PARTICLESYSTEMLAGRANGE_H
