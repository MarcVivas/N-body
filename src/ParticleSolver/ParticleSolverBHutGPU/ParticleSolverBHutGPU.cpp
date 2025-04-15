
#include "ParticleSolverBHutGPU.h"

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
    auto start_reset = std::chrono::high_resolution_clock::now();
    this->octree->reset(particles);
    glFinish();

    auto end_reset = std::chrono::high_resolution_clock::now();
    auto duration_reset = std::chrono::duration_cast<std::chrono::microseconds>(end_reset - start_reset);
    std::cout << "Time for reset: " << duration_reset.count() << " microseconds" << std::endl;

    //particles->gpuSort();


    auto start_insert = std::chrono::high_resolution_clock::now();
    this->octree->insert(particles);
    glFinish();

    auto end_insert = std::chrono::high_resolution_clock::now();
    auto duration_insert = std::chrono::duration_cast<std::chrono::microseconds>(end_insert - start_insert);
    std::cout << "Time for insert: " << duration_insert.count() << " microseconds" << std::endl;


    

    // Dispatch Compute Shader
    auto start_force = std::chrono::high_resolution_clock::now();
    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    this->forceCalculator->setFloat("G", this->G);
    this->forceCalculator->setFloat("theta", this->theta);
    this->forceCalculator->setFloat("squaredSoftening", this->squaredSoftening);
    this->forceCalculator->setInt("fatherTreeNodes", this->octree->getFatherTreeNodes());
    glDispatchCompute((particles->size()+1024-1) / 1024, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glFinish();
    auto end_force = std::chrono::high_resolution_clock::now();
    auto duration_force = std::chrono::duration_cast<std::chrono::microseconds>(end_force - start_force);
    std::cout << "Time for force: " << duration_force.count() << " microseconds" << std::endl;

  

    // Update particles positions
    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}


bool ParticleSolverBHutGPU::usesGPU() {return true;}



ParticleSolverBHutGPU::~ParticleSolverBHutGPU() = default;