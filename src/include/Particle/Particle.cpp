#include "Particle.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float m) :position(glm::vec4(pos, 0.f)), velocity(glm::vec4(vel, 0.f)), mass(m), acceleration(glm::vec4(0.f, 0.f, 0.f, 0.f)){};

Particle::Particle() = default;
Particle::~Particle() = default;

void Particle::setVelocity(glm::vec3 &vel) {
    this->velocity = glm::vec4(vel, 0.f);
}

std::ostream& operator<<(std::ostream& os, const Particle& p) {
    os << "Particle {" << std::endl
       << "  position: (" << p.position.x << ", " << p.position.y << ", " << p.position.z << ")" << std::endl
       << "  velocity: (" << p.velocity.x << ", " << p.velocity.y << ", " << p.velocity.z << ")" << std::endl
       << "  mass: " << p.mass << std::endl
       << "}";
    return os;
}