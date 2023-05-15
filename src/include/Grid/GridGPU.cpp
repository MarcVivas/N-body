#include "GridGPU.h"
#include <iostream>

GridGPU::GridGPU(std::string shaderPath, glm::vec3 worldDim, unsigned int bucketCap, unsigned int bucketsPerDim) {
    this->bucketsPerDimension = bucketsPerDim;
    this->totalBuckets = bucketsPerDimension * bucketsPerDimension * bucketsPerDimension;
    this->worldDimensions = glm::vec4(worldDim, 0);
    this->bucketCapacity = bucketCap;
    this->bucketSize = worldDimensions / (float) bucketsPerDimension;
    this->buckets = new Bucket*[this->totalBuckets];
    for (int i = 0; i < this->totalBuckets; i++) {
        this->buckets[i] = new Bucket(bucketCapacity, i);
    }
    minDimensions = glm::ivec4(0);
    maxDimensions = glm::ivec4((int) bucketsPerDimension - 1);
    this->gridShader = new ComputeShader(shaderPath);
}


void GridGPU::updateGrid(ParticleSystem *particles) {
    this->resetGrid();

    // Add the particles to their corresponding buckets
    for(unsigned int i = 0; i < particles->size(); i++){
        Bucket *bucket = this->getBucketByPosition(particles->getPositions()[i]);
        bucket->addParticle(particles, i);
    }


}

Bucket *GridGPU::getBucketByPosition(glm::vec4 particlePos) {
    const glm::ivec4 bucketCoords = glm::clamp(glm::ivec4(particlePos / bucketSize), minDimensions, maxDimensions);
    const unsigned int bucketIndex = bucketCoords.x + bucketCoords.y * bucketsPerDimension + bucketCoords.z * bucketsPerDimension * bucketsPerDimension;
    return this->buckets[bucketIndex];
}

Bucket *GridGPU::getBucketById(unsigned int bucketId) {
    return this->buckets[bucketId];
}

void GridGPU::resetGrid() {
    for(unsigned int i = 0; i < totalBuckets; i++){
        this->buckets[i]->resetBucket();
    }
}

unsigned int GridGPU::getTotalBuckets() {return this->totalBuckets;}

GridGPU::~GridGPU(){
    for(unsigned int i = 0; i < totalBuckets; i++) {
        delete this->buckets[i];
    }
    delete[] this->buckets;
}