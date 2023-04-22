#include <glm/glm.hpp>
#include <ostream>

#ifndef N_BODY_PARTICLE_H
#define N_BODY_PARTICLE_H


class Particle {
public:
    glm::vec4 position;
    glm::vec4 velocity;
    glm::vec4 acceleration;
    float mass;
    Particle();
    Particle(glm::vec3 pos, glm::vec3 vel, float mass);
    ~Particle();

    void updatePosition(float deltaTime, glm::vec3 newAcceleration);

    void setVelocity(glm::vec3 &vel);

    friend std::ostream& operator<<(std::ostream& os, const Particle& p);
};


#endif //N_BODY_PARTICLE_H
