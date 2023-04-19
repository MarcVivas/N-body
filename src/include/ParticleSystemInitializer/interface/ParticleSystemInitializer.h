//
// Created by marc on 12/03/23.
//

#ifndef PARTICLESYSTEMINITIALIZER_H
#define PARTICLESYSTEMINITIALIZER_H

#include <random>
#include <glm/gtc/random.hpp>
#include "../../ParticleSystem/ParticleSystem.cpp"

class ParticleSystemInitializer{
public:
    virtual ParticleSystem* generateParticles(glm::vec3 worldDimensions) = 0;
    virtual ~ParticleSystemInitializer() = default;
};

#endif //PARTICLESYSTEMINITIALIZER_H
