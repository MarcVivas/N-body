//
// Created by marc on 12/03/23.
//

#ifndef N_BODY_PARTICLESOLVER_H
#define N_BODY_PARTICLESOLVER_H


class ParticleSolver {
public:
    virtual ~ParticleSolver() = default;
    ParticleSolver() = default;
    virtual void updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities, float deltaTime) = 0;
    virtual bool usesGPU() = 0;
};


#endif //N_BODY_PARTICLESOLVER_H
