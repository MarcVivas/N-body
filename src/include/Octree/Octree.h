#include "ParticleSystem.h"
#include "Node.h"

#ifndef OCTREE_H
#define OCTREE_H



class Octree {
    public:
      Octree(){}
      Octree(int n);
      Node* nodes;
      int *parents;
      int parentCount;
      int maxNodes;
      int nodeCount;
      float theta;
      virtual void reset(ParticleSystem* p);
      virtual ~Octree();
      virtual void insert(glm::vec4 pos, glm::vec4 mass);
      virtual void propagate();
      virtual glm::vec4 computeGravityForce(glm::vec4& pos, float squaredSoftening, float G);
      virtual void prune();
      virtual Node *getNodes();
      virtual int getNodeCount();
      virtual int getMaxNodes();
      virtual void setNodes(Node *newNodes);
    protected:
      virtual void adjustBoundingBox(ParticleSystem* p);
      virtual void insertParticle(glm::vec4 pos, glm::vec4 mass, int i);
      virtual void subdivide(int i);
      virtual int getNextNode(glm::vec4 pos, int i);



};



#endif //OCTREE_H
