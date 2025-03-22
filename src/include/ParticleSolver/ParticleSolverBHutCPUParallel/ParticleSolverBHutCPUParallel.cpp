
#include "ParticleSolverBHutCPUParallel.h"

#include <iostream>

#include "ParallelOctreeCPU.h"
#include <glm/gtx/norm.hpp>

ParticleSolverBHutCPUParallel::ParticleSolverBHutCPUParallel(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new ParallelOctreeCPU(n);
}


bool ParticleSolverBHutCPUParallel::usesBH() {
    return true;
}

Octree* ParticleSolverBHutCPUParallel::getOctree() {
    return octree;
}
#include <chrono>
void ParticleSolverBHutCPUParallel::updateParticlePositions(ParticleSystem *particles){

    using namespace std::chrono;

    
    octree->reset(particles);

    // Time octree insert
    auto start_insert = high_resolution_clock::now();
    octree->insert(particles);
    auto end_insert = high_resolution_clock::now();
    auto duration_insert = duration_cast<milliseconds>(end_insert - start_insert); //milliseconds
    //std::cout << "Octree insert time: " << duration_insert.count() << " milliseconds" << std::endl;

    // Time gravity computation
    auto start_gravity = high_resolution_clock::now();
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < particles->size(); i++) {
        this->computeGravityForce(particles, i);
    }
    auto end_gravity = high_resolution_clock::now();
    auto duration_gravity = duration_cast<milliseconds>(end_gravity - start_gravity); //milliseconds
    //std::cout << "Gravity computation time: " << duration_gravity.count() << " milliseconds" << std::endl;



    #pragma omp parallel for schedule(static) 
    for(size_t i = 0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep);
    }

}

void
ParticleSolverBHutCPUParallel::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    particles->getForces()[particleId] = this->octree->computeGravityForce(particles->getPositions()[particleId], this->squaredSoftening, this->G);
}

bool ParticleSolverBHutCPUParallel::usesGPU() {return false;}


float ParticleSolverBHutCPUParallel::getSquaredSoftening() {
    return this->squaredSoftening;
}

ParticleSolverBHutCPUParallel::~ParticleSolverBHutCPUParallel() {
   delete this->octree;
}