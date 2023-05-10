#include "ParticleSystemSphere.h"
#include <glm/gtc/random.hpp>
#include <iostream>

ParticleSystemSphere::ParticleSystemSphere(size_t numParticles) : totalParticles(numParticles){}

ParticleSystem* ParticleSystemSphere::generateParticles(glm::vec3 worldDimensions) {
    Particle* particles = new Particle[this->totalParticles];

    float length = glm::length(worldDimensions) / 20.f;


    // Determine radius of sphere based on world dimensions
    float radius = glm::length(worldDimensions) / 2.0f;

    // Determine particle mass based on world volume and total particles
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float particleMass = (worldVolume / this->totalParticles) * 1.5;

    // Create particles distributed evenly on the surface of the sphere
    float a = 4.0f * M_PI / (float)this->totalParticles;
    float d = sqrtf(a);
    int mTheta = (int)roundf(M_PI / d);
    float dTheta = M_PI / (float)mTheta;
    float dPhi = a / dTheta;
    for (int m = 0, i = 0; m < mTheta; ++m) {
        float theta = M_PI * (float)(m + 0.5f) / (float)mTheta;
        int mPhi = (int)roundf(2.0f * M_PI * sinf(theta) / dPhi);
        for (int n = 0; n < mPhi; ++n, ++i) {
            float phi = 2.0f * M_PI * (float)n / (float)mPhi;
            glm::vec3 position(radius * sinf(theta) * cosf(phi), radius * sinf(theta) * sinf(phi), radius * cosf(theta));
            // Shift particle position by half of the world dimensions
            position += 0.5f * worldDimensions;
            if(i >= totalParticles){
                break;
            }
            glm::vec3 initialVel = (worldDimensions.z == 0) ? glm::linearRand(glm::vec3(-length, -length, 0.f), glm::vec3(length, length, 0.f)) : glm::linearRand(glm::vec3(-length, -length, -length), glm::vec3(length, length, length));
            particles[i] = Particle(position, initialVel, particleMass);
        }
    }

    std::vector<Particle> particleVector(particles, particles + this->totalParticles);

    delete[] particles; // free the allocated memory

    return new ParticleSystem(particleVector);
}