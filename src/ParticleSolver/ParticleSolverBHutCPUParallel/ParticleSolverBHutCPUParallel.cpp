
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



void ParticleSolverBHutCPUParallel::updateParticlePositions(ParticleSystem *particles){

   
    octree->reset(particles);

    // Time octree insert
    octree->insert(particles);


    // Time gravity computation
    #pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < particles->size(); i++) {
        this->computeGravityForce(particles, i);
    }




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