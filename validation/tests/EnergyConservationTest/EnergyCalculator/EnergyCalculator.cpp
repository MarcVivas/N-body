#include "EnergyCalculator.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

EnergyCalculator::EnergyCalculator(float size, float soft) {
    this->stepSize = size;
    this->softening = soft;
}


void EnergyCalculator::computeEnergy(ParticleSystem *particles) {

    // Compute energy and print it
    float kineticEnergy = 0.f;
    float potentialEnergy = 0.f;
    float G = 1.0f;

    for(size_t i = 0; i < particles->size(); i++){
        kineticEnergy += 0.5f * particles->getMasses()[i].x * glm::length2(particles->getVelocities()[i]);

        for(size_t j = i+1; j < particles->size(); j++){
            float squared_distance_i_j = glm::length2(particles->getPositions()[j] - particles->getPositions()[i]);
            potentialEnergy += (G * particles->getMasses()[i].x * particles->getMasses()[j].x) / std::sqrt(squared_distance_i_j + this->softening);
        }
    }


    std::cout << "Kinetic energy: " << kineticEnergy << '\n';
    std::cout << "Potential energy: " << -potentialEnergy << '\n';
    std::cout << "Total energy: " << kineticEnergy - potentialEnergy << '\n';

}