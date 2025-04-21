#include "ParticleSystemSphere.h"
#include <glm/gtc/random.hpp>
#include <iostream>

#include <random> // For C++ random number generation
#include <cmath>  // For M_PI (if needed, though not strictly required for this version)

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"   // For glm::vec3 generation helpers (optional, can use std::normal_distribution directly)
#include "glm/gtx/norm.hpp"     // For glm::normalize (ensure include path is correct)


ParticleSystemSphere::ParticleSystemSphere(size_t numParticles) : totalParticles(numParticles){}
std::mt19937& getRandomEngine() {
    static std::random_device rd; // Obtain a random number from hardware
    static std::mt19937 gen(rd()); // Seed the generator
    return gen;
}

std::unique_ptr<ParticleSystem> ParticleSystemSphere::generateParticles(
    const glm::vec3 worldDimensions, 
    const bool usesGPU)
{
    std::vector<Particle> particles;
    particles.reserve(this->totalParticles);

    // Get the seeded random number engine
    std::mt19937& gen = getRandomEngine();

    // --- Configuration ---
    // Radius of the hollow sphere
    float radius = glm::length(worldDimensions) / 3.0f;
    // Center of the sphere (usually the center of the world)
    glm::vec3 center = 0.5f * worldDimensions;

    // Base particle mass (consider adjusting the scaling factor)
    // Note: World volume might not be the most intuitive base for a surface distribution,
    // but it provides scaling with the world size. Adjust multiplier as needed.
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    // Prevent division by zero if totalParticles is 0
    float baseMass = (this->totalParticles > 0) ? (worldVolume / this->totalParticles) * 0.5f : 1.0f;
    // Add some mass variation for visual interest
    std::uniform_real_distribution<float> massDist(baseMass * 0.7f, baseMass * 1.3f);

    // Velocity configuration
    float baseVelocityMag = glm::length(worldDimensions) / 15.f; // Base magnitude for velocities
    std::uniform_real_distribution<float> velMagDist(baseVelocityMag * 0.5f, baseVelocityMag * 1.5f);
    std::normal_distribution<float> normDist(0.0f, 1.0f); // For generating points on sphere

    // --- Choose a Velocity Pattern ---
    enum class VelocityPattern { OUTWARD_EXPLOSION, INWARD_IMPLOSION, TANGENTIAL_ORBIT };

    // --- Choose a Velocity Pattern Randomly ---
    std::uniform_int_distribution<int> patternDist(0, 2); // Range for the 4 patterns
    VelocityPattern pattern = static_cast<VelocityPattern>(patternDist(gen)); // Randomly select pattern


    glm::vec3 orbitAxis = glm::normalize(glm::vec3(0.2f, 1.0f, -0.1f)); // Example axis for orbit (can be randomized too)


    // --- Particle Generation Loop ---
    for (int i = 0; i < totalParticles; ++i) {
        // 1. Generate Position (on the surface of the sphere)
        glm::vec3 position;
        float lenSq;
        do {
            // Generate a point using normal distribution for x, y, z
            // This ensures uniform distribution on the sphere surface after normalization
            position = glm::vec3(normDist(gen), normDist(gen), normDist(gen));
            lenSq = glm::dot(position, position);
        } while (lenSq < 1e-6); // Avoid potential division by zero if vector is ~zero (highly unlikely)

        position = glm::normalize(position) * radius; // Normalize and scale to desired radius
        position += center; // Translate to the correct center

        // 2. Generate Mass
        float particleMass = massDist(gen);

        // 3. Generate Velocity based on chosen pattern
        glm::vec3 initialVel(0.0f);
        float velMag = velMagDist(gen);
        glm::vec3 posRelativeToCenter = position - center; // Position vector from sphere center

        switch (pattern) {
        
        case VelocityPattern::OUTWARD_EXPLOSION: {
            // Velocity points directly away from the center
            if (glm::length(posRelativeToCenter) > 1e-6) { // Avoid normalizing zero vector
                initialVel = glm::normalize(posRelativeToCenter) * velMag;
            }
            break;
        }
        case VelocityPattern::INWARD_IMPLOSION: {
            // Velocity points directly towards the center
            if (glm::length(posRelativeToCenter) > 1e-6) { // Avoid normalizing zero vector
                initialVel = -glm::normalize(posRelativeToCenter) * velMag;
            }
            break;
        }
        case VelocityPattern::TANGENTIAL_ORBIT: {
            // Velocity is tangential to the sphere surface, creating an orbit/swirl
            glm::vec3 tangent = glm::cross(orbitAxis, posRelativeToCenter);
            if (glm::length(tangent) > 1e-6) { // Avoid normalizing zero vector (if pos is parallel to axis)
                initialVel = glm::normalize(tangent) * velMag;
            }
            else {
                // Handle cases where position is along the axis (poles of rotation)
                // Give a small random velocity instead or choose another tangent logic
                glm::vec3 randDir;
                do {
                    randDir = glm::vec3(normDist(gen), normDist(gen), normDist(gen));
                    lenSq = glm::dot(randDir, randDir);
                } while (lenSq < 1e-6);
                initialVel = glm::normalize(randDir) * (velMag * 0.1f); // Small random velocity
            }
            break;
        }
        }

        // Optional: Add a small random jitter to the calculated velocity
        // float jitterScale = baseVelocityMag * 0.1f;
        // glm::vec3 jitter(normDist(gen), normDist(gen), normDist(gen));
        // if (glm::length(jitter) > 1e-6) {
        //    initialVel += glm::normalize(jitter) * jitterScale;
        // }


       // 4. Create Particle
        particles.emplace_back(position, initialVel, particleMass); // Use emplace_back
    }

    return std::make_unique<ParticleSystem>(std::move(particles), usesGPU); // Move particles vector
}