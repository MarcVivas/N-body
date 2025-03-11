

#include "ParallelOctreeGPU.h"

ParallelOctreeGPU::ParallelOctreeGPU(int n): Octree(n){
}

ParallelOctreeGPU::~ParallelOctreeGPU() {
}

void ParallelOctreeGPU::insert(ParticleSystem *p) {
    int insertedParticles = 0;
    while (insertedParticles != p->size()) {

    }

}
