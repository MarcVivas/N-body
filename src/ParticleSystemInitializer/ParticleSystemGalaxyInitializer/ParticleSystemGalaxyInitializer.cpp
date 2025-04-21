#include "ParticleSystemGalaxyInitializer.h"
#include <random>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

ParticleSystemGalaxyInitializer::ParticleSystemGalaxyInitializer(size_t numParticles) :totalParticles(numParticles) {
}



std::unique_ptr<ParticleSystem> ParticleSystemGalaxyInitializer::generateParticles(glm::vec3 worldDimensions, const bool usesGPU) {
    std::vector<Particle> particles;
    particles.reserve(this->totalParticles); // Reserve space

    // --- Centralized Random Number Generation ---
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed); // Use this generator for ALL random numbers below

    // --- Random Choice: 50/50 between the two logic styles ---
    std::bernoulli_distribution fiftyFiftyDist(0.5); // 50% probability
    bool executeFirstLogicStyle = fiftyFiftyDist(rng);

    glm::vec3 center = worldDimensions / 2.f;
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;

    // Handle potentially zero volume (e.g., 2D case) - Robustness check
    if (worldVolume <= 1e-9f) { // Use a small epsilon
         // Try calculating based on area if one dimension is zero
        if (worldDimensions.x > 1e-9f && worldDimensions.y > 1e-9f && worldDimensions.z <= 1e-9f) {
             worldVolume = worldDimensions.x * worldDimensions.y * 0.1f; // Arbitrary thickness factor
        } else if (worldDimensions.x > 1e-9f && worldDimensions.z > 1e-9f && worldDimensions.y <= 1e-9f) {
             worldVolume = worldDimensions.x * worldDimensions.z * 0.1f;
        } else if (worldDimensions.y > 1e-9f && worldDimensions.z > 1e-9f && worldDimensions.x <= 1e-9f) {
             worldVolume = worldDimensions.y * worldDimensions.z * 0.1f;
        } else {
            // Fallback based on largest dimension if multiple are zero or calculation fails
            float maxDim = glm::max(worldDimensions.x, glm::max(worldDimensions.y, worldDimensions.z));
             if (maxDim <= 1e-9f) maxDim = 1.0f; // Ultimate fallback if all dimensions are zero
             worldVolume = maxDim * maxDim * maxDim * 0.1f; // Crude cubic estimate
        }
    }
     if (worldVolume <= 1e-9f) worldVolume = 1.0f; // Final fallback


    if (executeFirstLogicStyle) {
        std::mt19937 mt(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> randRadius(0.f, worldDimensions.x);
        std::uniform_real_distribution<float> randThickness(0.1f, worldDimensions.z / 5.f);
        std::normal_distribution<float> randTheta(0.0f, 3.1415f * 50);
        float length = glm::length(worldDimensions) / 4.f;
    
        float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
        float particleMass = (worldVolume / this->totalParticles) * 0.5;
    
        // Define the parameters for the bulge
        float bulgeRadius = worldDimensions.x / 10.f;
    
        // Define the parameters for the spiral arms
        float armRadius = worldDimensions.x / 2.f;
    
        glm::vec3 center = glm::vec3(worldDimensions.x / 2.f, worldDimensions.y / 2.f, worldDimensions.z / 2.f);
    
        for (size_t i = 0; i < totalParticles; i++) {
            float radius = randRadius(mt);
            float theta = randTheta(mt) * 2.f * glm::pi<float>();
            float height = (worldDimensions.z == 0) ? 0 : randThickness(mt);
            glm::vec3 particlePos = center + glm::vec3(radius * std::cos(theta), radius * std::sin(theta), height);
    
            // Compute the mass based on the particle's position
            if (radius < bulgeRadius) {
                particleMass = (worldVolume / this->totalParticles) * 0.7;
            } else {
                particleMass = (worldVolume / this->totalParticles) * 0.5;
            }
    
            glm::vec3 initialVel = glm::vec3(-length * sin(theta), length * cos(theta), 0.f);
    
            Particle newParticle(particlePos, initialVel, particleMass);
    
            if (radius < armRadius) {
                // Calculate the angle between the particle's position vector and the radial vector
                float angle = std::atan2(particlePos.y - center.y, particlePos.x - center.x);
                float armVelocityMagnitude = length * (radius / armRadius) * std::cos(6.f * (angle - theta));
                glm::vec3 armVelocity = glm::vec3(armVelocityMagnitude * -cos(theta), armVelocityMagnitude * -sin(theta), 0.f);
                glm::vec3 newVel = initialVel + armVelocity;
                newParticle.setVelocity(newVel);
            }
            particles.push_back(newParticle); // Add the particle to the vector
        }

    } else {
        // ---====================================================---
        // ---== Branch 2: Execute New Randomized Logic Branch ==---
        // ---====================================================---
        // (This is the logic from the 'else' block in the second snippet)

        // --- Random Choice: Stability Preference (only for this branch) ---
        std::bernoulli_distribution stabilityDist(0.7);
        bool favorStability = stabilityDist(rng);

        // --- Randomize Galaxy Structure Parameters ---
        float maxDim = glm::max(worldDimensions.x, glm::max(worldDimensions.y, worldDimensions.z));
        if (maxDim <= 1e-9f) maxDim = 100.0f; // Fallback if dimensions are zero

        std::uniform_real_distribution<float> randBulgeFactor(0.05f, 0.15f);
        float bulgeRadius = maxDim * randBulgeFactor(rng);

        std::uniform_real_distribution<float> randDiskFactor(0.3f, 0.5f);
        float diskRadius = maxDim * randDiskFactor(rng); // Use this as the max radius for particle generation

        std::uniform_real_distribution<float> randThicknessFactor(0.02f, 0.10f);
        float diskThickness = diskRadius * randThicknessFactor(rng);
        // Force flat if Z dimension is zero or negative
        if (worldDimensions.z <= 1e-9f) diskThickness = 0.f;
        // Ensure positive thickness if Z is positive
        else if (diskThickness <= 1e-9f) diskThickness = 0.01f * diskRadius; // Small minimum thickness

        std::uniform_int_distribution<int> randNumArms(2, 6);
        float numArms = static_cast<float>(randNumArms(rng));

        std::uniform_real_distribution<float> randArmTightness(0.5f, 2.0f);
        float armTightness = randArmTightness(rng);

        // --- Determine Velocity Parameter Ranges based on Stability Preference ---
        float velScaleMin, velScaleMax, armStrengthMin, armStrengthMax, dispersionFactor;
        if (favorStability) { // Lower energy ranges
            velScaleMin = 0.5f; velScaleMax = 0.9f;
            armStrengthMin = 0.05f; armStrengthMax = 0.2f;
            dispersionFactor = 0.05f;
        } else { // Higher energy ranges allowed
            velScaleMin = 0.8f; velScaleMax = 1.5f;
            armStrengthMin = 0.1f; armStrengthMax = 0.5f;
            dispersionFactor = 0.15f;
        }
        std::uniform_real_distribution<float> randVelScale(velScaleMin, velScaleMax);
        std::uniform_real_distribution<float> randArmStrengthNew(armStrengthMin, armStrengthMax); // Use unique name

        float baseVelocityMagnitude = (diskRadius / 2.0f) * randVelScale(rng);
        float dispersionScale = baseVelocityMagnitude * dispersionFactor;
        std::normal_distribution<float> randDispersion(0.0f, dispersionScale > 1e-9f ? dispersionScale : 1e-6f); // Avoid zero std dev

        // --- Randomize Other Parameters (New Style) ---
        std::uniform_real_distribution<float> randMassFactorNew(0.8f, 1.2f);
        float baseParticleMassNew = (this->totalParticles > 0) ? (worldVolume / this->totalParticles) : 1.0f;
        if (baseParticleMassNew <= 1e-9f) baseParticleMassNew = 1e-6f; // Avoid zero mass
        float bulgeMassFactorNew = 1.5f;

        // --- Distributions for Individual Particle Properties (New Style) ---
        // Generate radius more concentrated towards center
        std::uniform_real_distribution<float> randUnit(0.f, 1.f);
        float radiusPower = 0.5f; // Power < 1 concentrates towards center (0), > 1 towards edge (diskRadius)

        std::uniform_real_distribution<float> randAngleNew(0.f, 2.f * glm::pi<float>());

        // Centered height distribution for the new style
        std::normal_distribution<float> randHeightNew(0.0f, diskThickness > 1e-9f ? diskThickness / 3.0f : 0.f); // StdDev = 1/3rd thickness, avoid zero std dev if flat

        // --- Particle Generation Loop (New Randomized Logic) ---
        for (size_t i = 0; i < totalParticles; ++i) {
            // Position (New Style - power distribution for radius)
            float u = randUnit(rng);
            float radius = diskRadius * std::pow(u, radiusPower); // Max radius is diskRadius
            float theta = randAngleNew(rng);
            // Height is normally distributed around the galactic plane (center.z)
            float height_offset = (diskThickness <= 1e-9f) ? 0.f : randHeightNew(rng);
            glm::vec3 particlePos = center + glm::vec3(radius * std::cos(theta), radius * std::sin(theta), height_offset);

            // Mass (New Style - bulge factor + individual randomness)
            float particleMass = baseParticleMassNew;
            if (radius < bulgeRadius) { // Compare generated radius to randomized bulgeRadius
                particleMass *= bulgeMassFactorNew;
            }
            particleMass *= randMassFactorNew(rng); // Apply individual random factor

            // Velocity Calculation (New Style: Circular Profile + Log Arms + Dispersion)
            // Simple velocity profile (increases then optionally caps)
            float velMagnitude = baseVelocityMagnitude * glm::min(radius / (bulgeRadius + 1e-6f), 1.5f); // Add epsilon to avoid div by zero
            glm::vec3 circularVel = glm::vec3(-velMagnitude * std::sin(theta), velMagnitude * std::cos(theta), 0.f);

            // Arm Perturbation (New Style)
            float armStrength = randArmStrengthNew(rng); // Use the stability-adjusted range
            // Avoid log(<=0) issues with bulgeRadius or radius close to 0
            float logRadiusInput = glm::max(radius / (bulgeRadius + 1e-6f), 1.0f); // Ensure input >= 1.0
            float spiralPhase = armTightness * std::log(logRadiusInput);
            float armAngleEffect = numArms * (theta - spiralPhase); // Use particle's generated theta
            float armVelMagnitude = armStrength * velMagnitude * std::cos(armAngleEffect);

            // Perturbation direction (mostly radial for this style)
            glm::vec3 armPerturbation = glm::vec3(0.0f);
            glm::vec3 radialDir = particlePos - center;
            radialDir.z = 0; // Project to XY plane
            if (glm::length(radialDir) > 1e-6f) { // Avoid normalizing zero vector
                armPerturbation = armVelMagnitude * glm::normalize(radialDir);
            }

            // Velocity Dispersion (New Style)
            glm::vec3 dispersionVel = glm::vec3(randDispersion(rng), randDispersion(rng), randDispersion(rng) * 0.5f); // Less Z dispersion

            // Combine velocities
            glm::vec3 finalVel = circularVel + armPerturbation + dispersionVel;

            Particle newParticle(particlePos, finalVel, particleMass);
            particles.push_back(newParticle);
        }
    } // End of if/else for logic choice

    // Return the ParticleSystem, passing the usesGPU flag
    return std::make_unique<ParticleSystem>(particles, usesGPU);
}