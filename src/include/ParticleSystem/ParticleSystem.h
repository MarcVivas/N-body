//
// Created by marc on 19/04/23.
//

#ifndef N_BODY_PARTICLESYSTEM_H
#define N_BODY_PARTICLESYSTEM_H


class ParticleSystem {
public:
    ParticleSystem(std::vector<Particle> &particles);
    ~ParticleSystem() = default;
    void updateParticlePosition(unsigned int particleId, float deltaTime, glm::vec3 newAcceleration);
    unsigned int size();
    glm::vec4* getPositions();
    glm::vec4* getVelocities();
    glm::vec4* getAccelerations();
    glm::vec4* getMasses();
    void setMasses(glm::vec4* newMasses);
    void setPositions(glm::vec4* newPositions);
    void setAccelerations(glm::vec4* newAccelerations);
    void setVelocities(glm::vec4* newVelocities);
    friend std::ostream& operator<<(std::ostream& os, const ParticleSystem& system);

protected:

    unsigned int numParticles;
    glm::vec4* positions;
    glm::vec4* accelerations;
    glm::vec4* velocities;
    glm::vec4* masses;

    // Declare ParticleSimulation as a friend class
    friend class ParticleSimulation;

};


#endif //N_BODY_PARTICLESYSTEM_H
