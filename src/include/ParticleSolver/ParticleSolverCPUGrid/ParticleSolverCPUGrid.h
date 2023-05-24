
#include "ParticleSolver.h"
#include "GridCPU.h"

#ifndef N_BODY_PARTICLESOLVERCPUGRID_H
#define N_BODY_PARTICLESOLVERCPUGRID_H


class ParticleSolverCPUGrid: public ParticleSolver  {
public:
    ParticleSolverCPUGrid(GridCPU *gridCpu, float timeStep, float squaredSoftening);
    ~ParticleSolverCPUGrid();
    void updateParticlePositions(ParticleSystem *particles) override;
    bool usesGPU() override;
    float getSquaredSoftening() override;
protected:
    float timeStep;
    float squaredSoftening;
    void computeGravityForce(ParticleSystem *particles, const unsigned int particleId);
    float G;
    GridCPU *gridCpu;

};


#endif //N_BODY_PARTICLESOLVERCPUPARALLEL_H
