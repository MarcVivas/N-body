
#include "ParticleSolverBarnesHut.h"

#include <chrono>
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <Octree.h>

ParticleSolverBarnesHut::ParticleSolverBarnesHut(float stepSize, float squaredSoft, int n): ParticleSolver() {
    this->squaredSoftening = squaredSoft;
    this->timeStep = stepSize;
    this->G = 1.0f;
    this->octree = new Octree(n);
}


void ParticleSolverBarnesHut::updateParticlePositions(ParticleSystem *particles){
    using namespace std::chrono; // For easier access to chrono types

        auto start_reset = high_resolution_clock::now();
        this->octree->reset(particles);
        auto end_reset = high_resolution_clock::now();
        auto duration_reset = duration_cast<microseconds>(end_reset - start_reset);

        auto start_insert = high_resolution_clock::now();
        for(size_t i = 0; i < particles->size(); i++){
          this->octree->insert(particles->getPositions()[i], particles->getMasses()[i]);
        }
        auto end_insert = high_resolution_clock::now();
        auto duration_insert = duration_cast<microseconds>(end_insert - start_insert);

        auto start_propagate = high_resolution_clock::now();
        this->octree->propagate();
        auto end_propagate = high_resolution_clock::now();
        auto duration_propagate = duration_cast<microseconds>(end_propagate - start_propagate);

        auto start_force = high_resolution_clock::now();
        for(size_t i = 0; i<particles->size(); i++){
            this->computeGravityForce(particles, i);
        }
        auto end_force = high_resolution_clock::now();
        auto duration_force = duration_cast<microseconds>(end_force - start_force);

        auto start_update = high_resolution_clock::now();
        for(size_t i = 0; i<particles->size(); i++){
            particles->updateParticlePosition(i, this->timeStep);
        }
        auto end_update = high_resolution_clock::now();
        auto duration_update = duration_cast<microseconds>(end_update - start_update);

        std::cout << "--- Time Profiling for updateParticlePositions ---" << std::endl;
        std::cout << "Octree Reset Time:     " << duration_reset.count() << " microseconds" << std::endl;
        std::cout << "Octree Insert Time:    " << duration_insert.count() << " microseconds" << std::endl;
        std::cout << "Octree Propagate Time: " << duration_propagate.count() << " microseconds" << std::endl;
        std::cout << "Force Compute Time:    " << duration_force.count() << " microseconds" << std::endl;
        std::cout << "Position Update Time:  " << duration_update.count() << " microseconds" << std::endl;
        std::cout << "----------------------------------------------" << std::endl;
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