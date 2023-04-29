#include "ParticleSystemInitializer.h"

#ifndef N_BODY_PARTICLESYSTEMLAGRANGE_H
#define N_BODY_PARTICLESYSTEMLAGRANGE_H


class ParticleSystemLagrange: public ParticleSystemInitializer {
public:
    ParticleSystemLagrange();
    ParticleSystem* generateParticles(glm::vec3 worldDimensions);

private:
    size_t totalParticles;

};


#endif //N_BODY_PARTICLESYSTEMLAGRANGE_H
