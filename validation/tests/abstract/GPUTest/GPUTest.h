#include "../../interface/Test.h"
#include "../../../../src/include/ParticleSolver/interface/ParticleSolver.h"
#include "../../../../src/include/ParticleSolver/ParticleSolverGPU/ParticleSolverGPU.cpp"

#ifndef N_BODY_GPUTEST_H
#define N_BODY_GPUTEST_H


class GPUTest : public Test{
public:
    GPUTest(ParticleSystem* particleSystem, std::string& shaderPath);
    virtual ~GPUTest();
    virtual void runTest(const size_t iterations, const double stepSize) = 0;

protected:
    ParticleSystem *particles;
    ParticleSolver *solver;
    GLuint postitions_SSBO, velocities_SSBO, accelerations_SSBO;
    GLFWwindow* window;
    virtual void updateParticleSystemBuffers();

};


#endif //N_BODY_GPUTEST_H
