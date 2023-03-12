//
// Created by marc on 12/03/23.
//

#include "../interface/ParticleSystemInitializer.h"

#ifndef PARTICLESYSTEMCUBEINITIALIZER_H
#define PARTICLESYSTEMCUBEINITIALIZER_H


class ParticleSystemCubeInitializer: public ParticleSystemInitializer{

public:
    ParticleSystemCubeInitializer(size_t numParticles);
    std::vector<Particle> generateParticles();

private:
    size_t totalParticles;
};


#endif //PARTICLESYSTEMCUBEINITIALIZER_H
