//
// Created by marc on 30/03/23.
//
#include "../interface/ParticleSolver.h"

#ifndef N_BODY_PARTICLESOLVERGPU_H
#define N_BODY_PARTICLESOLVERGPU_H


class ParticleSolverGPU: public ParticleSolver {
public:
    ParticleSolverGPU(std::string &pathToComputeShader);
    ~ParticleSolverGPU();
    bool usesGPU() override;
    void updateParticlePositions(std::vector<Particle> &particles, glm::vec4* positions, glm::vec4* velocities,  float deltaTime) override;
private:
    Shader *computeShader;

};


#endif //N_BODY_PARTICLESOLVERGPU_H
