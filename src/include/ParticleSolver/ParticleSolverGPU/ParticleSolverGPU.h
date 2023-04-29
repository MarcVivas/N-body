
#include "ParticleSolver.h"
#include "ComputeShader.h"

#ifndef N_BODY_PARTICLESOLVERGPU_H
#define N_BODY_PARTICLESOLVERGPU_H


class ParticleSolverGPU: public ParticleSolver {
public:
    ParticleSolverGPU(float stepSize, float squaredSoft, std::string &pathToComputeShader);
    ~ParticleSolverGPU();
    bool usesGPU() override;
    void updateParticlePositions(ParticleSystem *particles) override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
    Shader *computeShader;

};


#endif //N_BODY_PARTICLESOLVERGPU_H
