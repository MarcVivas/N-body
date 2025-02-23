
#include "ParticleSolver.h"
#include "ComputeShader.h"

#ifndef N_BODY_PARTICLESOLVERGPU_H
#define N_BODY_PARTICLESOLVERGPU_H


class ParticleSolverGPU: public ParticleSolver {
public:
    ParticleSolverGPU(float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    ParticleSolverGPU(double block_size, float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    ~ParticleSolverGPU();
    bool usesGPU() override;
    void updateParticlePositions(ParticleSystem *particles) override;
    float getSquaredSoftening() override;
    bool usesBH() override;
    Octree* getOctree() override;
protected:
    float squaredSoftening;
    Shader *positionCalculator, *forceCalculator;
    double blockSize;
};


#endif //N_BODY_PARTICLESOLVERGPU_H
