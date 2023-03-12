//
// Created by marc on 5/03/23.
//

// ParticleSystem AbstractClass

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
class ParticleSystem {
public:
    virtual void update(double deltaTime) = 0;
    virtual void draw() = 0;

    ParticleSystem(size_t numParticles, InitializationType initType, ParticleSolver *particleSysSolver);
    virtual ~ParticleSystem();
    std::vector<Particle> particles;
private:
    void generateCubeSystem();
    void generateGalaxySystem();

    friend std::ostream& operator<<(std::ostream& os, const ParticleSystem& ps) {
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
};
#endif // PARTICLESYSTEM_H
