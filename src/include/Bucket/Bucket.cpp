#include "Bucket.h"
#include <iostream>

Bucket::Bucket(unsigned int bucketCapacity, unsigned int id) {
    this->bucketId = id;
    this->particlesIds = new glm::uvec4[bucketCapacity];
    this->centerOfMass = glm::vec4(0);
    this->numParticles = glm::uvec4(0);
}

Bucket::Bucket() = default;

Bucket::~Bucket() {
    delete [] this->particlesIds;
}

void Bucket::addParticle(ParticleSystem* particles, unsigned int particleId) {
    this->particlesIds[this->numParticles.x].x = particleId;
    this->numParticles.x += 1;

    glm::vec4 particlePos = particles->getPositions()[particleId];

    this->centerOfMass.x += particlePos.x;
    this->centerOfMass.y += particlePos.y;
    this->centerOfMass.z += particlePos.z;
    this->centerOfMass.w += particles->getMasses()[particleId].x;
}

unsigned int Bucket::getNumParticles() {
    return this->numParticles.x;
}

unsigned int Bucket::getParticleId(const unsigned int pos) {
    return this->particlesIds[pos].x;
}

glm::vec4 Bucket::getCenterOfMass(){
    if(numParticles.x > 0){
        return {
                centerOfMass.x / float(numParticles.x),
                centerOfMass.y / float(numParticles.x),
                centerOfMass.z / float(numParticles.x),
                centerOfMass.w
                };
    }
    return glm::vec4(0);
}

unsigned int Bucket::getBucketId(){
    return this->bucketId;
}

void Bucket::resetBucket() {
    this->centerOfMass = glm::vec4(0);
    this->numParticles = glm::uvec4(0);
}