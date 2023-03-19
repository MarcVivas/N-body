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
    unsigned int VAO;
    unsigned int VBO;
    GLsync gSync = nullptr;
    Particle* particlesPinnedMemory;
    Camera* camera;

    void lockParticlesBuffer();
    void waitParticlesBuffer();
    void copyParticlesToGPU();

};
#endif // PARTICLESIMULATION_H
