#include "ParticleSystem.h"


#ifndef PARTICLESYSTEMINITIALIZER_H
#define PARTICLESYSTEMINITIALIZER_H


class ParticleSystemInitializer{
public:
    virtual ParticleSystem* generateParticles(glm::vec3 worldDimensions) = 0;
    virtual ~ParticleSystemInitializer() = default;
};

#endif //PARTICLESYSTEMINITIALIZER_H
