//
// Created by marc on 7/03/23.
//

#ifndef N_BODY_PARTICLE_H
#define N_BODY_PARTICLE_H


class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    Particle(glm::vec3 pos, glm::vec3 vel, float mass);
    ~Particle();

    friend std::ostream& operator<<(std::ostream& os, const Particle& p) {
        os << "Particle {" << std::endl
           << "  position: (" << p.position.x << ", " << p.position.y << ", " << p.position.z << ")" << std::endl
           << "  velocity: (" << p.velocity.x << ", " << p.velocity.y << ", " << p.velocity.z << ")" << std::endl
           << "  mass: " << p.mass << std::endl
           << "}";
        return os;
    }
};


#endif //N_BODY_PARTICLE_H
