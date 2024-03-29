#include "Test.h"
#include <string>
#include "ParticleSystem.h"
#include "ParticleSolverGPU.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifndef N_BODY_GPUTEST_H
#define N_BODY_GPUTEST_H


class GPUTest : public Test{
public:
    GPUTest(ParticleSystem* particleSystem, std::string& positionsShaderPath, std::string& forcesShaderPath, float stepSize, float softening);
    virtual ~GPUTest();
    virtual void runTest(const size_t iterations) = 0;
    virtual void resetSSBO();

protected:
    ParticleSystem *particles;
    ParticleSolver *solver;
    GLuint postitions_SSBO, velocities_SSBO, accelerations_SSBO, forces_SSBO, masses_SSBO;
    GLFWwindow* window;
    virtual void updateParticleSystemBuffers();

};


#endif //N_BODY_GPUTEST_H
