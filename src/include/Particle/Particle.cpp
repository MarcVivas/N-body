//
// Created by marc on 7/03/23.
//

#include "Particle.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float m) :position(pos), velocity(vel), mass(m), acceleration(glm::vec3(0, 0, 0)){};

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
    this->acceleration = newAcceleration;

    // Compute next velocity (i + 1)
    this->velocity += this->acceleration * dtDividedBy2;
}
