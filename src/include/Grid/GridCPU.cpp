#include "GridCPU.h"
#include <iostream>
#include <omp.h>

GridCPU::GridCPU(glm::vec3 worldDim, unsigned int bucketCap, unsigned int bucketsPerDim) {
    this->bucketsPerDimension = bucketsPerDim;
    this->totalBuckets = bucketsPerDimension * bucketsPerDimension * bucketsPerDimension;
    this->worldDimensions = glm::vec4(worldDim, 0);
    this->bucketCapacity = bucketCap;
    this->bucketSize = worldDimensions / (float) bucketsPerDimension;
    this->buckets = new Bucket*[this->totalBuckets];
    bucketLocks.reserve(totalBuckets);
    for (int i = 0; i < this->totalBuckets; i++) {
        this->buckets[i] = new Bucket(bucketCapacity, i);
        omp_init_lock(&bucketLocks[i]);
    }
    minDimensions = glm::ivec4(0);
    maxDimensions = glm::ivec4((int) bucketsPerDimension - 1);
}


void GridCPU::updateGrid(ParticleSystem *particles) {
    this->resetGrid();

    // Add the particles to their corresponding buckets
    #pragma omp parallel for schedule(static)
    for(unsigned int i = 0; i < particles->size(); i++){
        Bucket *bucket = this->getBucketByPosition(particles->getPositions()[i]);
        omp_set_lock(&bucketLocks[bucket->getBucketId()]);
        bucket->addParticle(particles, i);
        omp_unset_lock(&bucketLocks[bucket->getBucketId()]);
    }


}

Bucket *GridCPU::getBucketByPosition(glm::vec4 particlePos) {
    const glm::ivec4 bucketCoords = glm::clamp(glm::ivec4(particlePos / bucketSize), minDimensions, maxDimensions);
    const unsigned int bucketIndex = bucketCoords.x + bucketCoords.y * bucketsPerDimension + bucketCoords.z * bucketsPerDimension * bucketsPerDimension;
    return this->buckets[bucketIndex];
}

Bucket *GridCPU::getBucketById(unsigned int bucketId) {
    return this->buckets[bucketId];
}

void GridCPU::resetGrid() {
#pragma omp parallel for schedule(static) shared(buckets)
    for(unsigned int i = 0; i < totalBuckets; i++){
        this->buckets[i]->resetBucket();
    }
}

unsigned int GridCPU::getTotalBuckets() {return this->totalBuckets;}

GridCPU::~GridCPU(){
    for(unsigned int i = 0; i < totalBuckets; i++) {
        delete this->buckets[i];
        omp_destroy_lock(&bucketLocks[i]);
    }
    delete[] this->buckets;
}