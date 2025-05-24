#include "ParticleSystem.h"
#include "Helpers.h"
#include "MortonCode.h"
#include <bitset>
using helpers::log;



ParticleSystem::ParticleSystem(const std::vector<Particle> &particles, const bool usesGPU) {
    this->numParticles = particles.size();
    this->velocities = new glm::vec4[this->numParticles]();
    this->accelerations = new glm::vec4[this->numParticles]();
    this->positions = new glm::vec4[this->numParticles]();
    this->masses = new glm::vec4[this->numParticles]();
    this->forces = new glm::vec4[this->numParticles]();


    for (unsigned int i = 0; i < this->numParticles; i++) {
        this->velocities[i] = particles[i].velocity;
        this->accelerations[i] = particles[i].acceleration;
        this->positions[i] = particles[i].position;
        this->masses[i] = glm::vec4(particles[i].mass, 0, 0, 0);
        this->forces[i] = glm::vec4(0.f);
    }

    this->createBuffers(usesGPU);
	this->compileShaders();
}

void ParticleSystem::compileShaders() {
    mortonShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/morton.glsl"));
    rearrangeParticlesShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/rearrange.glsl"));
	bitonicSortShader = std::make_unique<ComputeShader>(std::string("../src/shaders/ComputeShaders/bitonicSort.glsl"));
}

ParticleSystem::ParticleSystem(ParticleSystem * other) {
    this->numParticles = other->size();
    this->velocities = new glm::vec4[this->numParticles]();
    this->accelerations = new glm::vec4[this->numParticles]();
    this->positions = new glm::vec4[this->numParticles]();
    this->masses = new glm::vec4[this->numParticles]();
    this->forces = new glm::vec4[this->numParticles]();

    for (unsigned int i = 0; i < this->numParticles; i++) {
        this->velocities[i] = other->getVelocities()[i];
        this->accelerations[i] = other->getAccelerations()[i];
        this->positions[i] = other->positions[i];
        this->masses[i] = other->masses[i];
        this->forces[i] = other->forces[i];
    }
}


void ParticleSystem::createBuffers(bool usesGPU) {
    if (usesGPU) {
        this->configureGpuBuffers();
    }
    else {
        this->configureCpuBuffers();
    }
}

unsigned int ParticleSystem::nextPowerOfTwo(unsigned int n){
    if (n == 0) return 1; // Or handle as needed
    // Simple bit manipulation method
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;

}

void ParticleSystem::configureGpuBuffers() {
	this->paddedNumParticles = nextPowerOfTwo(this->numParticles);
    positions_SSBO.createBufferData(sizeof(glm::vec4) * this->size(), this->getPositions(), 0, GL_DYNAMIC_COPY);
    velocities_SSBO.createBufferData(sizeof(glm::vec4) * this->size(), this->getVelocities(), 1, GL_DYNAMIC_COPY);
    accelerations_SSBO.createBufferData(sizeof(glm::vec4) * this->size(), this->getAccelerations(), 2, GL_DYNAMIC_COPY);
    masses_SSBO.createBufferData(sizeof(glm::vec4) * this->size(), this->getMasses(), 3, GL_STATIC_DRAW);
    forces_SSBO.createBufferData(sizeof(glm::vec4) * this->size(), this->getForces(), 4, GL_DYNAMIC_COPY);
    mortonBuffer.createBufferData(sizeof(MortonCode) * paddedNumParticles, nullptr, 16, GL_DYNAMIC_COPY);
    positionsCopy.createBufferData(sizeof(glm::vec4) * this->size(), nullptr, 17, GL_DYNAMIC_COPY);
    velocitiesCopy.createBufferData(sizeof(glm::vec4) * this->size(), nullptr, 18, GL_DYNAMIC_COPY);
}

/**
 * Creates persistent mapped shader storage buffer objects
 */
void ParticleSystem::configureCpuBuffers() {
    GLbitfield bufferStorageFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    positions_SSBO.createBufferStorage(sizeof(glm::vec4) * this->size(), this->getPositions(), 0, bufferStorageFlags);
    glm::vec4* positions = (glm::vec4*)positions_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->size(), bufferStorageFlags);
    this->setPositions(positions);

    velocities_SSBO.createBufferStorage(sizeof(glm::vec4) * this->size(), this->getVelocities(), 1, bufferStorageFlags);
    glm::vec4* velocities = (glm::vec4*)velocities_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->size(), bufferStorageFlags);
    this->setVelocities(velocities);

    accelerations_SSBO.createBufferStorage(sizeof(glm::vec4) * this->size(), this->getAccelerations(), 2, bufferStorageFlags);
    glm::vec4* accelerations = (glm::vec4*)accelerations_SSBO.mapBufferRange(0, sizeof(glm::vec4) * this->size(), bufferStorageFlags);
    this->setAccelerations(accelerations);
}

// Sort the particles in the GPU using the z-order curve
// Why? Improve cache locality, memory access patterns and reduce warp divergence
void ParticleSystem::gpuSort() {
	static bool pingPong = true;

	OpenGLBuffer* readPositions = pingPong ? &positions_SSBO : &positionsCopy;
	OpenGLBuffer* readVelocities = pingPong ? &velocities_SSBO : &velocitiesCopy;
	OpenGLBuffer* writePositions = pingPong ? &positionsCopy : &positions_SSBO;
	OpenGLBuffer* writeVelocities = pingPong ? &velocitiesCopy : &velocities_SSBO;



    // Calculate morton codes
    mortonShader->use();
	readPositions->bindBufferBase(0);
	readVelocities->bindBufferBase(1);
	mortonBuffer.bindBufferBase(16);
    mortonShader->setInt("actualNumParticles", this->numParticles);
	mortonShader->setInt("paddedNumParticles", this->paddedNumParticles);
	glDispatchCompute((this->paddedNumParticles + 256 - 1) / 256, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);



	// Sort morton codes using bitonic sort (implement radix sort for better performance)
	bitonicSortShader->use();
    readPositions->bindBufferBase(0);
    readVelocities->bindBufferBase(1);
    mortonBuffer.bindBufferBase(16);
	bitonicSortShader->setInt("numParticles", this->paddedNumParticles);

    // Bitonic sort requires numParticles to be a power of 2 for the simplest
    // implementation. If not, you either need to pad your buffers/particle count
    // to the next power of two OR modify the shader/dispatch logic to handle
    // the exact size (more complex bounds checking).

    const static unsigned int N = this->paddedNumParticles; // Or use next power of 2 if padding

    for (unsigned int k = 2; k <= N; k <<= 1) { // k = size of sequences being merged
        bitonicSortShader->setInt("k", k);
        for (unsigned int j = k >> 1; j > 0; j >>= 1) { // j = comparison distance
            if (j < 1) continue; // Should not happen with k >= 2
            bitonicSortShader->setInt("j", j);

            glDispatchCompute((this->paddedNumParticles + 256 - 1) / 256, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }


	// Rearrange particles based on sorted morton codes
	rearrangeParticlesShader->use();
    readPositions->bindBufferBase(0);
    readVelocities->bindBufferBase(1);
    mortonBuffer.bindBufferBase(16);
	writePositions->bindBufferBase(17);
	writeVelocities->bindBufferBase(18);
	rearrangeParticlesShader->setInt("numParticles", this->numParticles);
	glDispatchCompute((this->numParticles + 256 - 1) / 256, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	pingPong = !pingPong;
	writePositions->bindBufferBase(0);
	writeVelocities->bindBufferBase(1);
	readPositions->bindBufferBase(17);
	readVelocities->bindBufferBase(18);

	// auto morton = mortonBuffer.getDataVector<MortonCode>(0, size()*sizeof(MortonCode));
	// auto pos = readPositions->getDataVector<glm::vec4>(0, this->size()*sizeof(glm::vec4));
	// for(auto &i: morton){

	//     glm::vec4 p = pos[i.index];
	//     log(p, i.morton, i.index, std::bitset<64>(i.morton));
	// }


}








void ParticleSystem::setAccelerations(glm::vec4 *newAccelerations) {
    delete [] this->accelerations;
    this->accelerations = newAccelerations;
}

void ParticleSystem::setMasses(glm::vec4 *newMasses) {
    delete [] this->masses;
    this->masses = newMasses;
}

void ParticleSystem::setForces(glm::vec4 *newForces) {
    delete [] this->forces;
    this->forces = newForces;
}

void ParticleSystem::setPositions(glm::vec4 *newPositions) {
    delete [] this->positions;
    this->positions = newPositions;
}

void ParticleSystem::setVelocities(glm::vec4 *newVelocities) {
    delete [] this->velocities;
    this->velocities = newVelocities;
}

std::ostream& operator<<(std::ostream& os, const ParticleSystem& system) {
    os << "Particle System with " << system.numParticles << " particles:" << std::endl;
    for (unsigned int i = 0; i < system.numParticles; ++i) {
        os << "Particle ID: " << i << std::endl;
        os << "Position: (" << system.positions[i].x << ", " << system.positions[i].y << ", " << system.positions[i].z << ")" << std::endl;
        os << "Velocity: (" << system.velocities[i].x << ", " << system.velocities[i].y << ", " << system.velocities[i].z << ")" << std::endl;
        os << "Acceleration: (" << system.accelerations[i].x << ", " << system.accelerations[i].y << ", " << system.accelerations[i].z << ")" << std::endl;
        os << "Mass: " << system.masses[i].x << std::endl;
    }
    return os;
}
