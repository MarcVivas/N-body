#include "GPUTest.h"
#include <iostream>

GPUTest::GPUTest(ParticleSystem* particleSystem, std::string& shaderPath, float stepSize, float softening): Test(){
    this->particles = particleSystem;

    // Initialize GLFW
    glfwInit();

    // Create a hidden window
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    this->window = glfwCreateWindow(1, 1, "", nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }


    // Shader storage buffer objects

    // This SSBO stores particles positions
    glGenBuffers(1, &this->postitions_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, this->postitions_SSBO);

    // This SSBO stores particles velocities
    glGenBuffers(1, &this->velocities_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, this->velocities_SSBO);

    // This SSBO stores particles accelerations
    glGenBuffers(1, &this->accelerations_SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, this->accelerations_SSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles->size(), this->particles->getPositions(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles->size(), this->particles->getVelocities(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * this->particles->size(), this->particles->getAccelerations(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    this->solver = new ParticleSolverGPU(stepSize, softening, shaderPath);


}

void GPUTest::updateParticleSystemBuffers() {
    // Bind the position SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->postitions_SSBO);
    // Map the position SSBO memory to CPU-accessible memory
    glm::vec4* positions = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    int numParticles = this->particles->size();
    glm::vec4* copiedPositions = new glm::vec4[numParticles];
    std::copy(positions, positions + numParticles, copiedPositions);
    this->particles->setPositions(copiedPositions);
    // Unmap the position SSBO
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Bind the velocity SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->velocities_SSBO);
    // Map the velocity SSBO memory to CPU-accessible memory
    glm::vec4* velocities = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    glm::vec4* copiedVelocities = new glm::vec4[numParticles];
    std::copy(velocities, velocities + numParticles, copiedVelocities);
    this->particles->setVelocities(copiedVelocities);
    // Unmap the position SSBO
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Bind the acceleration SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->accelerations_SSBO);
    // Map the acceleration SSBO memory to CPU-accessible memory
    glm::vec4* accelerations = (glm::vec4*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    glm::vec4* copiedAccelerations = new glm::vec4[numParticles];
    std::copy(accelerations, accelerations + numParticles, copiedAccelerations);
    this->particles->setAccelerations(copiedAccelerations);
    // Unmap the position SSBO
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    // Unbind the SSBOs
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


GPUTest::~GPUTest() noexcept {
    delete this->solver;
    glfwTerminate();
}