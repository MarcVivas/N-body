
#include "ParticleSolverBHutCPUParallel.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBHutCPUHybrid::ParticleSolverBHutCPUHybrid(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);
}


bool ParticleSolverBHutCPUHybrid::usesBH() {
    return true;
}

Octree* ParticleSolverBHutCPUHybrid::getOctree() {
    return octree;
}

void ParticleSolverBHutCPUHybrid::updateParticlePositions(ParticleSystem *particles){

    this->octree->reset(particles);


    for(size_t i = 0; i < particles->size(); i++){
        this->octree->insert(particles->getPositions()[i], particles->getMasses()[i]);
    }


    this->octree->propagate();


    this->octree->prune();




    #pragma omp parallel for schedule(static) shared(particles)
    for(size_t i = 0; i<particles->size(); i++){
        this->computeGravityForce(particles, i);
    }


    #pragma omp parallel for schedule(static) shared(particles)
    for(size_t i = 0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep);
    }

}

void
ParticleSolverBHutCPUHybrid::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    particles->getForces()[particleId] = this->octree->computeGravityForce(particles->getPositions()[particleId], this->squaredSoftening, this->G);
}

bool ParticleSolverBHutCPUHybrid::usesGPU() {return false;}


float ParticleSolverBHutCPUHybrid::getSquaredSoftening() {
    return this->squaredSoftening;
}

ParticleSolverBHutCPUHybrid::~ParticleSolverBHutCPUHybrid() {
   delete this->octree;
}