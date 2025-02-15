
#include "ParticleSolverBarnesHut.h"
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBarnesHut::ParticleSolverBarnesHut(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);
}


void ParticleSolverBarnesHut::updateParticlePositions(ParticleSystem *particles){
    this->octree->reset(particles);

    for(size_t i = 0; i < particles->size(); i++){
      this->octree->insert(particles->getPositions()[i], particles->getMasses()[i]);
    }

    this->octree->propagate();

    for(size_t i = 0; i<particles->size(); i++){
        this->computeGravityForce(particles, i);
    }
    for(size_t i = 0; i<particles->size(); i++){
        particles->updateParticlePosition(i, this->timeStep);
    }
}

void
ParticleSolverBarnesHut::computeGravityForce(ParticleSystem *particles, const unsigned int particleId) {
    particles->getForces()[particleId] = this->octree->computeGravityForce(particles->getPositions()[particleId], this->squaredSoftening, this->G);
}

bool ParticleSolverBarnesHut::usesGPU() {return false;}


float ParticleSolverBarnesHut::getSquaredSoftening() {
    return this->squaredSoftening;
}

ParticleSolverBarnesHut::~ParticleSolverBarnesHut() {
   delete this->octree;
}