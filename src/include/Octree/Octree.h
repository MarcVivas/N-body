#include "ParticleSystem.h"
#include "Node.h"

#ifndef OCTREE_H
#define OCTREE_H



class Octree {
    public:
      Octree(int n);
      Node* nodes;
      int *parents;
      int parentCount;
      int maxNodes;
      int nodeCount;
      float theta;
      void reset(ParticleSystem* p);
      ~Octree();
      void insert(glm::vec4 pos, glm::vec4 mass);
      void propagate();
      glm::vec4 computeGravityForce(glm::vec4& pos, float squaredSoftening, float G);
      void prune();
      Node *getNodes();
      int getNodeCount();
      int getMaxNodes();
    private:
      void adjustBoundingBox(ParticleSystem* p);
      void insertParticle(glm::vec4 pos, glm::vec4 mass, int i);
      void subdivide(int i);
      void getQuadrant(glm::vec4 pos);

      /**
       * Get the next node based on the quadrant the particle is in
       * @param pos
       * @param i
       * @return
       */
      inline int getNextNode(glm::vec4 pos, int i){
        const glm::vec4 quadCenter = this->nodes->getQuadrantCenter(i);
        const int quadrantIndex = (pos.x < quadCenter.x) | (pos.y < quadCenter.y) << 1 | (pos.z < quadCenter.z) << 2;
        return this->nodes->getFirstChild(i) + quadrantIndex;
      }



};



#endif //OCTREE_H
