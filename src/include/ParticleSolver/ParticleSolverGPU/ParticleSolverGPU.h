
#include "ParticleSolver.h"
#include "ComputeShader.h"

#ifndef N_BODY_PARTICLESOLVERGPU_H
#define N_BODY_PARTICLESOLVERGPU_H


class ParticleSolverGPU: public ParticleSolver {
public:
    ParticleSolverGPU(std::string &pathToComputeShader);
    ~ParticleSolverGPU();
    bool usesGPU() override;
    void updateParticlePositions(ParticleSystem *particles, float deltaTime) override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
    Shader *computeShader;

};


#endif //N_BODY_PARTICLESOLVERGPU_H
