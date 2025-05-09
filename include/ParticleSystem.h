

#ifndef N_BODY_PARTICLESYSTEM_H
#define N_BODY_PARTICLESYSTEM_H
#include <glm/glm.hpp>
#include <vector>
#include <ostream>
#include <Particle.h>
#include <ComputeShader.h>
#include <memory>
#include <OpenGLBuffer.h>

class ParticleSystem {
public:
    ParticleSystem(const std::vector<Particle> &particles, const bool usesGPU);
    ParticleSystem(ParticleSystem* other);

    void setMasses(glm::vec4* newMasses);
    void setForces(glm::vec4* newForces);
    void setPositions(glm::vec4* newPositions);
    void setAccelerations(glm::vec4* newAccelerations);
    void setVelocities(glm::vec4* newVelocities);
    friend std::ostream& operator<<(std::ostream& os, const ParticleSystem& system);

    void gpuSort();


    /**
    * Updates a particle position
    * Performs the leapfrog integration
    * @param particleId
    * @param deltaTime
    */
    void updateParticlePosition(unsigned int particleId, float deltaTime) {
        float dtDividedBy2 = deltaTime * 0.5f;

        // Compute velocity (i + 1/2)
        this->velocities[particleId] += this->accelerations[particleId] * dtDividedBy2;

        // Compute next position (i+1)
        this->positions[particleId] += this->velocities[particleId] * deltaTime;

        // Update acceleration (i+1)
        // F = MA;
        // A = F/M;  M is cancelled when calculating gravity force
        this->accelerations[particleId] = this->forces[particleId];

        // Compute next velocity (i+1)
        this->velocities[particleId] += this->accelerations[particleId] * dtDividedBy2;

    }

    unsigned int size()  {
        return this->numParticles;
    }

    glm::vec4* getMasses() {
        return this->masses;
    }

    glm::vec4* getPositions() const{
        return this->positions;
    }

    glm::vec4* getVelocities() const {
        return this->velocities;
    }

    glm::vec4* getAccelerations() const {
        return this->accelerations;
    }

    glm::vec4* getForces() const {
        return this->forces;
    }

   
protected:
    // OPENGL OWNS THE POINTERS
    unsigned int numParticles;
	unsigned int paddedNumParticles;
    glm::vec4* positions;
    glm::vec4* accelerations;
    glm::vec4* velocities;
    glm::vec4* masses;
    glm::vec4* forces;

	std::unique_ptr<Shader> mortonShader;
    std::unique_ptr<Shader> rearrangeParticlesShader; 
    std::unique_ptr<Shader> bitonicSortShader;

    OpenGLBuffer mortonBuffer, positionsCopy, velocitiesCopy;
    OpenGLBuffer positions_SSBO, velocities_SSBO, accelerations_SSBO, masses_SSBO, forces_SSBO, nodes_SSBO;

    // Declare ParticleSimulation as a friend class
    friend class ParticleSimulation;
    
    
    void configureGpuBuffers();
    void configureCpuBuffers();
    void createBuffers(bool usesGPU);
	void compileShaders();
    unsigned int nextPowerOfTwo(unsigned int n);

};


#endif //N_BODY_PARTICLESYSTEM_H
