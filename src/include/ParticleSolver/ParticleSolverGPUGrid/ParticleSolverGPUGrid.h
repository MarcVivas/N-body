
#include "ParticleSolver.h"
#include "ComputeShader.h"
#include "GridGPU.h"

#ifndef N_BODY_PARTICLESOLVERGPUGRID_H
#define N_BODY_PARTICLESOLVERGPUGRID_H


class ParticleSolverGPUGrid: public ParticleSolver {
public:
    ParticleSolverGPUGrid(GridGPU *gridGpu, float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    ParticleSolverGPUGrid(GridGPU *gridGpu, double block_size, float stepSize, float squaredSoft, std::string &positionCalculatorPath, std::string &forceCalculatorPath);
    ~ParticleSolverGPUGrid();
    bool usesGPU() override;
    void updateParticlePositions(ParticleSystem *particles) override;
    float getSquaredSoftening() override;
protected:
    float squaredSoftening;
    Shader *positionCalculator, *forceCalculator;
    double blockSize;
    GridGPU *grid;
};


#endif //N_BODY_PARTICLESOLVERGPUGRID_H
