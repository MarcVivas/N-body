
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
    this->octree->reset(particles);


    // It doesn't work right now for large number of particles
	// I think the problem is in the insert function (executeTasks)
	// Don't know why it's happening and I am tired of debugging 
	// In theory, if the particles are sorted, the performance should be better
	// Reducing warp divergence and improving cache locality
    //particles->gpuSort();




    this->octree->insert(particles);



    this->forceCalculator->use();
    this->forceCalculator->setInt("numParticles", particles->size());
    this->forceCalculator->setFloat("G", this->G);
    this->forceCalculator->setFloat("theta", this->theta);
    this->forceCalculator->setFloat("squaredSoftening", this->squaredSoftening);
    this->forceCalculator->setInt("fatherTreeNodes", this->octree->getFatherTreeNodes());
    glDispatchCompute((particles->size()+1024-1) / 1024, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


  

    // Update particles positions
    this->positionCalculator->use();
    this->positionCalculator->setInt("numParticles", particles->size());
    glDispatchCompute(ceil(particles->size() / 64.0), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}


bool ParticleSolverBHutGPU::usesGPU() {return true;}



ParticleSolverBHutGPU::~ParticleSolverBHutGPU() = default;