
#include "ParticleSolverBHutCPUParallel.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBHutCPUParallel::ParticleSolverBHutCPUParallel(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);
}


bool ParticleSolverBHutCPUParallel::usesBH() {
    return true;
}

Octree* ParticleSolverBHutCPUParallel::getOctree() {
    return octree;
}

void ParticleSolverBHutCPUParallel::updateParticlePositions(ParticleSystem *particles){

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