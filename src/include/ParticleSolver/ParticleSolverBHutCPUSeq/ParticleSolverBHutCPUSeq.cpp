
#include "ParticleSolverBHutCPUSeq.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBHutCPUSeq::ParticleSolverBHutCPUSeq(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);
}


void ParticleSolverBHutCPUSeq::updateParticlePositions(ParticleSystem *particles){

        this->octree->reset(particles);

        for(size_t i = 0; i < particles->size(); i++){
          this->octree->insert(particles->getPositions()[i], particles->getMasses()[i]);
        }

        this->octree->propagate();

        this->octree->prune();


        for(size_t i = 0; i<particles->size(); i++){
            this->computeGravityForce(particles, i);
        }


        for(size_t i = 0; i<particles->size(); i++){
            particles->updateParticlePosition(i, this->timeStep);
        }

}

bool ParticleSolverBHutCPUSeq::usesBH() {
    return true;
}
Octree* ParticleSolverBHutCPUSeq::getOctree() {
    return octree;
}
void
ParticleSolverBHutCPUSeq::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    particles->getForces()[particleId] = this->octree->computeGravityForce(particles->getPositions()[particleId], this->squaredSoftening, this->G);
}

bool ParticleSolverBHutCPUSeq::usesGPU() {return false;}


float ParticleSolverBHutCPUSeq::getSquaredSoftening() {
    return this->squaredSoftening;
}

ParticleSolverBHutCPUSeq::~ParticleSolverBHutCPUSeq() {
   delete this->octree;
}