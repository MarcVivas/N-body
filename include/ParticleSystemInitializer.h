

#ifndef PARTICLESYSTEMINITIALIZER_H
#define PARTICLESYSTEMINITIALIZER_H
#include "ParticleSystem.h"
#include <memory>

class ParticleSystemInitializer{
public:
    virtual std::unique_ptr<ParticleSystem> generateParticles(glm::vec3 worldDimensions) = 0;
    virtual ~ParticleSystemInitializer() = default;
};

#endif //PARTICLESYSTEMINITIALIZER_H
