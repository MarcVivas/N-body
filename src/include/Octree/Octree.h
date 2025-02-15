#include "ParticleSystem.h"
#include "Node.h"

#ifndef OCTREE_H
#define OCTREE_H



class Octree {
    public:
      Octree(int n);
      Node* nodes;
      size_t *parents;
      size_t parentCount;
      size_t maxNodes;
      int nodeCount;
      float theta;
      void reset(ParticleSystem* p);
      ~Octree();
      void insert(glm::vec4 pos, glm::vec4 mass);
      void propagate();
      glm::vec4 computeGravityForce(glm::vec4 pos, const float squaredSoftening, const float G);

    private:
      void adjustBoundingBox(ParticleSystem* p);
      void insertParticle(glm::vec4 pos, glm::vec4 mass, size_t i);
      size_t getNextNode(glm::vec4 pos, size_t i);
      void subdivide(size_t i);
      void getQuadrant(glm::vec4 pos);


};



#endif //OCTREE_H
