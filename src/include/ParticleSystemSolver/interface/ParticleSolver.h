//
// Created by marc on 12/03/23.
//

#ifndef N_BODY_PARTICLESOLVER_H
#define N_BODY_PARTICLESOLVER_H


class ParticleSolver {
public:
    virtual ~ParticleSolver() = default;
    ParticleSolver() = default;
    virtual void updateParticlePositions(std::vector<Particle> &particles, float deltaTime) = 0;
};


#endif //N_BODY_PARTICLESOLVER_H
