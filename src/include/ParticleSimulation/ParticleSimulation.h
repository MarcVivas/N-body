//
// Created by marc on 5/03/23.
//

#ifndef PARTICLESIMULATION_H
#define PARTICLESIMULATION_H
class ParticleSimulation {
public:
    virtual void update(double deltaTime);
    virtual void draw();
    virtual Camera* getCamera();
    virtual std::vector<glm::vec4> getPositions();
    virtual std::vector<glm::vec4> getVelocities();
    virtual std::vector<glm::vec4> getAccelerations();
    ParticleSimulation(ParticleSystemInitializer *particleSystemInitializer, ParticleSolver *particleSysSolver, glm::vec3 worldDim, glm::vec2 windowDim);
    ~ParticleSimulation();
private:

    friend std::ostream& operator<<(std::ostream& os, const ParticleSimulation& ps) {
        os << "ParticleSystem {" << std::endl
           << "  numParticles: " << ps.particles.size() << std::endl
           << "  particles: " << std::endl;
        for (const auto& p : ps.particles) {
            os << p << std::endl;
        }
        os << "}";
        return os;
    }

protected:
    ParticleSolver *particleSolver;
    glm::vec3 worldDimensions;
    std::vector<Particle> particles;
    Shader *renderShader;
    GLuint VAO;
    GLuint VBO;
    GLsync gSync = nullptr;
    glm::vec4* particlesPositions;
    glm::vec4* particlesVelocities;
    glm::vec4* particlesAccelerations;
    Camera* camera;
    GLuint postitions_SSBO, velocities_SSBO, accelerations_SSBO;
    Bloom *bloom;

    void lockParticlesBuffer();
    void waitParticlesBuffer();
    void copyParticlesToGPU();
    void createBuffers(bool usesGPU);
    void configureGpuBuffers();
    void configureCpuBuffers();

};
#endif // PARTICLESIMULATION_H
