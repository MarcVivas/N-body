//
// Created by marc on 7/03/23.
//

#include "Particle.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float m) :position(glm::vec4(pos, 0.f)), velocity(glm::vec4(vel, 0.f)), mass(m), acceleration(glm::vec4(0.f, 0.f, 0.f, 0.f)){};

Particle::Particle() = default;
Particle::~Particle() = default;

/**
 * Updates the particle position
 * Performs the leapfrog integration
 * @param dt
 * @param newAcceleration
 */
void Particle::updatePosition(float deltaTime, glm::vec3 newAcceleration) {
    float dtDividedBy2 = deltaTime/2.f;

    // Compute velocity (i + 1/2)
    this->velocity += this->acceleration * dtDividedBy2;

    // Compute next position (i+1)
    this->position += this->velocity * deltaTime;

    // Update acceleration (i+1)
    this->acceleration = glm::vec4(newAcceleration, 0);

    // Compute next velocity (i + 1)
    this->velocity += this->acceleration * dtDividedBy2;
}

void Particle::setVelocity(glm::vec3 &vel) {
    this->velocity = glm::vec4(vel, 0.f);
}