#include "Octree.h"

#ifndef PARALLELOCTREEGPU_H
#define PARALLELOCTREEGPU_H



class ParallelOctreeGPU: public Octree {
    ParallelOctreeGPU(int n);
    virtual ~ParallelOctreeGPU();
    void insert(ParticleSystem *p);

};



#endif //PARALLELOCTREEGPU_H
