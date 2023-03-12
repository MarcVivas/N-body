//
// Created by marc on 12/03/23.
//

#ifndef PARTICLESYSTEMINITIALIZER_H
#define PARTICLESYSTEMINITIALIZER_H

class ParticleSystemInitializer{
public:
    virtual std::vector<Particle> generateParticles() = 0;
};

#endif //PARTICLESYSTEMINITIALIZER_H
