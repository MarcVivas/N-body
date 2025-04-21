#include "ParticleSystemBall.h"
#include <random>   // For std::mt19937, std::random_device, distributions
#include <cmath>    // For std::acos, std::sin, std::cos, std::pow
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp> // For glm::pi
#include <iostream>

ParticleSystemBall::ParticleSystemBall(size_t numParticles) : totalParticles(numParticles){}

std::unique_ptr<ParticleSystem> ParticleSystemBall::generateParticles(glm::vec3 worldDimensions, const bool usesGPU) {
    std::vector<Particle> particles;
    particles.reserve(this->totalParticles);

    // --- Randomness Setup ---
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    std::uniform_real_distribution<float> dist_neg1_1(-1.0f, 1.0f); // For perturbations

    // --- Parameters ---
    float placementRadius = glm::length(worldDimensions) / 3.0f; // Sphere within which to place particles
    glm::vec3 worldCenter = 0.5f * worldDimensions;

    // --- CRITICAL: Initial Velocity Scale ---
    // Start EXTREMELY low or zero. Gravity needs to dominate.
    // Adjust this based on your simulation's G and mass scale.
    // If it still explodes, make this even smaller or exactly 0.0f.
    float velocityScale = glm::length(worldDimensions) * 0.001f; // MUCH smaller scale
    // float velocityScale = 0.0f; // Option: Start with zero velocity

    // --- Mass Distribution ---
    float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
    float averageMass = (worldVolume > 1e-6f && this->totalParticles > 0)
        ? (worldVolume / static_cast<float>(this->totalParticles)) * 1.0f
        : 1.0f;
    // Adjust mass range if needed, ensure it's meaningful relative to G
    float minMass = averageMass * 0.5f;
    float maxMass = averageMass * 5.0f; // Reduced max range slightly, less extreme contrast initially
    float massExponent = 2.0f; // Skew towards lower masses
    auto generateMass = [&]() -> float {
        float u = dist01(gen);
        return minMass + (maxMass - minMass) * std::pow(u, massExponent);
        };


    // --- Position Generation Strategy: Perturbed Uniform Sphere ---
    // We'll generate points uniformly within the sphere, then add a small random nudge.
    // This provides the initial density fluctuations gravity needs to form structure.
    float perturbationScale = placementRadius * 0.05f; // How much to nudge particles (e.g., 5% of radius)

    for (int i = 0; i < totalParticles; ++i) {

        // --- Generate Position (Uniformly in Sphere) ---
        // Use rejection sampling or proper spherical coords for uniform *volume*
        glm::vec3 pos_relative;
        float r_sq;
        do {
            pos_relative.x = dist_neg1_1(gen) * placementRadius;
            pos_relative.y = dist_neg1_1(gen) * placementRadius;
            pos_relative.z = (worldDimensions.z == 0) ? 0.0f : dist_neg1_1(gen) * placementRadius;
            r_sq = glm::dot(pos_relative, pos_relative);
        } while (r_sq > placementRadius * placementRadius); // Reject points outside sphere

        // --- Add Perturbation ---
        glm::vec3 perturbation;
        perturbation.x = dist_neg1_1(gen) * perturbationScale;
        perturbation.y = dist_neg1_1(gen) * perturbationScale;
        perturbation.z = (worldDimensions.z == 0) ? 0.0f : dist_neg1_1(gen) * perturbationScale;
        pos_relative += perturbation;

        // Ensure particle stays within world bounds if necessary (optional)
        // This might clip perturbations near the edge. Consider if this is desired.
        glm::vec3 position = worldCenter + pos_relative;
        // position = glm::clamp(position, glm::vec3(0.0f), worldDimensions);


        // --- Generate Velocity (Very Low or Zero) ---
        glm::vec3 initialVel(0.0f); // Default to zero
        if (velocityScale > 1e-9f) { // Only calculate if scale is non-zero
            initialVel.x = dist_neg1_1(gen) * velocityScale;
            initialVel.y = dist_neg1_1(gen) * velocityScale;
            initialVel.z = (worldDimensions.z == 0) ? 0.0f : (dist_neg1_1(gen) * velocityScale);
        }

        // --- Generate Mass ---
        float mass = generateMass();

        // --- Add Particle ---
        particles.emplace_back(position, initialVel, mass);
    }

    return std::make_unique<ParticleSystem>(particles, usesGPU);
}