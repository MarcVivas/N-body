#include "ParticleSystem.h"
#include "Bucket.h"

#ifndef N_BODY_GRID_H
#define N_BODY_GRID_H



class Grid {
public:
    virtual ~Grid() = default;
    Grid() = default;
    Bucket **buckets;
    glm::vec4 worldDimensions;
    unsigned int bucketsPerDimension;
    glm::vec4 bucketSize;
    glm::ivec4 maxDimensions;
    glm::ivec4 minDimensions;
    unsigned int bucketCapacity;
    unsigned int totalBuckets;
    virtual void updateGrid(ParticleSystem *particleSystem) = 0;
    virtual Bucket* getBucketByPosition(glm::vec4 particlePos) = 0;
    virtual Bucket* getBucketById(unsigned int bucketId) = 0;
    virtual unsigned int getTotalBuckets() = 0;

private:
    virtual void resetGrid() = 0;
};


#endif //N_BODY_GRID_H
