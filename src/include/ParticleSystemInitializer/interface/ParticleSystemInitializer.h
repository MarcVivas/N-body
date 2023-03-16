//
// Created by marc on 12/03/23.
//

#ifndef PARTICLESYSTEMINITIALIZER_H
#define PARTICLESYSTEMINITIALIZER_H

class ParticleSystemInitializer{
public:
    virtual std::vector<Particle> generateParticles(glm::vec3 worldDimensions) = 0;
    virtual ~ParticleSystemInitializer() = default;
};

#endif //PARTICLESYSTEMINITIALIZER_H
