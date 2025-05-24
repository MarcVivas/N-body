
#include "ParticleSolverBHutGPU.h"
#include "ParallelOctreeGPU.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>
#include <ComputeShader.h>

ParticleSolverBHutGPU::ParticleSolverBHutGPU(float stepSize, float squaredSoft, int n, std::string &positionCalculatorPath, std::string &forceCalculatorPath): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = std::make_unique<ParallelOctreeGPU>(n);

    this->positionCalculator = std::make_unique<ComputeShader>(positionCalculatorPath);
    this->positionCalculator->use();
    this->positionCalculator->setFloat("deltaTime", stepSize);

    this->forceCalculator = std::make_unique<ComputeShader>(forceCalculatorPath);
    this->forceCalculator->use();
    this->forceCalculator->setFloat("squaredSoftening", squaredSoft);
}

void ParticleSolverBHutGPU::updateParticlePositions(ParticleSystem *particles){
    auto start = std::chrono::high_resolution_clock::now();
    this->octree->reset(particles);
    glFinish();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Octree reset time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us" << std::endl;

    // It doesn't work right now for large number of particles
	// I think the problem is in the insert function (executeTasks)
	// Don't know why it's happening and I am tired of debugging
	// In theory, if the particles are sorted, the performance should be better
	// Reducing warp divergence and improving cache locality

	start = std::chrono::high_resolution_clock::now();
    particles->gpuSort();
    glFinish();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "GPU sort time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us" << std::endl;


    start = std::chrono::high_resolution_clock::now();
    this->octree->insert(particles);
    glFinish();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Octree insert time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us" << std::endl;


    start = std::chrono::high_resolution_clock::now();

    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    this->forceCalculator->setFloat("G", this->G);
    this->forceCalculator->setFloat("theta", this->theta);
    this->forceCalculator->setFloat("squaredSoftening", this->squaredSoftening);
    glDispatchCompute((particles->size()+255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glFinish();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Force time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us" << std::endl;




    // Update particles positions
    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glFinish();

}


bool ParticleSolverBHutGPU::usesGPU() {return true;}



ParticleSolverBHutGPU::~ParticleSolverBHutGPU() = default;
