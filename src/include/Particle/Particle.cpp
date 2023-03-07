//
// Created by marc on 7/03/23.
//

#include "Particle.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float m) :position(pos), velocity(vel), mass(m){};

Particle::~Particle() = default;
