#include "EnergyCalculator.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

void EnergyCalculator::computeEnergy(ParticleSystem *particles, const float squaredSoftening) {

    // Compute energy and print it
    float kineticEnergy = 0.f;
    float potentialEnergy = 0.f;
    float G = 6.6743e-11;

    for(size_t j = 0; j < particles->size(); j++){
        kineticEnergy += 0.5f * particles->getMasses()[j].x * glm::length2(particles->getVelocities()[j]);

        for(size_t k = 0; k < particles->size(); k++){
            float squared_distance_i_j = glm::length2(particles->getPositions()[k] - particles->getPositions()[j]);
            potentialEnergy += (G * particles->getMasses()[j].x * particles->getMasses()[k].x) / std::sqrt(squared_distance_i_j + squaredSoftening);
        }
    }


    std::cout << "Kinetic energy: " << kineticEnergy << '\n';
    std::cout << "Potential energy: " << -potentialEnergy << '\n';
    std::cout << "Total energy: " << kineticEnergy - potentialEnergy << '\n';

}