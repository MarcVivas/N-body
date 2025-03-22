#include "Octree.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

/**
 * Constructor
 * @param n number of particles
 */
Octree::Octree(int n){
  this->maxNodes = n < 200 ? n*200 : n*8;
  this->nodes = new Node[this->maxNodes];
  this->parents = new int[this->maxNodes];
  this->nodeCount = 0;
  this->parentCount = 0;
  this->theta = 0.5f *0.5f;
}

/**
 * Establishes the bounding box of the root node
 * O(n)
 * @param p Particles
 */
void Octree::adjustBoundingBox(ParticleSystem* p){
  float min_x = std::numeric_limits<float>::max();
  float min_y = min_x;
  float min_z = min_x;
  float max_x = std::numeric_limits<float>::min();
  float max_y = max_x;
  float max_z = max_x;

  // Find the max and min positions
  for (int i = 0;  i < p->size(); ++i){
    const glm::vec4 pos = p->getPositions()[i];

    if (pos.x < min_x){
      min_x = pos.x;
    }
    if (pos.y < min_y){
      min_y = pos.y;
    }
    if (pos.z < min_z){
      min_z = pos.z;
    }
    if (pos.x > max_x){
      max_x = pos.x;
    }
    if (pos.y > max_y){
      max_y = pos.y;
    }
    if (pos.z > max_z){
      max_z = pos.z;
    }
  }

  // Set the bounding box of the root node
  this->nodes[0].setBoundingBox(glm::vec4(min_x, min_y, min_z, 0.f), glm::vec4(max_x, max_y, max_z, 0.f));
}

/**
 * Clears the tree and recomputes the root's bounding box
 * O(n)
 * @param p Particles
 */
void Octree::reset(ParticleSystem* p){
  this->adjustBoundingBox(p);
  Node &root = this->nodes[0];
  root.setFirstChild(-1);
  root.setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f));
  this->nodeCount = 1;
  this->parentCount = 0;
  root.setNext(-1);
}


/**
 * Inserts a particle into the octree (iteratively and stackless)
 * Average O(n log(n))
 * @param pos Particle position
 * @param mass Particle mass
 */
void Octree::insert(glm::vec4 pos, glm::vec4 mass, int root){
  // Start at the root of the octree
  int i = root;

  while(i < this->maxNodes){
    Node &node = this->nodes[i];

    // Case 1: The node is an empty leaf
    if(node.isLeaf() && !node.isOccupied()){
      // Proceed to insert the particle
      this->insertParticle(pos, mass, i);
      return;
    }

    // Case 2: The node is an occupied leaf.
    if(node.isLeaf() && node.isOccupied()){
      // Get the current particle's position and mass
      glm::vec4 pos2 = node.getCenterOfMass();
      glm::vec4 mass2 = glm::vec4(node.getMass(), 0.f, 0.f, 0.f);

      // Subdivide the current node
      this->subdivide(i);

      // Get child indexes
      int childIndex1 = this->getNextNode(pos, i);
      int childIndex2 = this->getNextNode(pos2, i);
      const float dist = glm::length2(pos2-pos);

      // If both particles go to the same child node we need to keep subdividing
      while (childIndex1 == childIndex2) {
        // Edge case: Both particles are in the same position or very close
        if (dist < 1e-6) {
          this->insertParticle(pos, glm::vec4(mass2.x + mass.x, 0.f, 0.f, 0.f), childIndex1);  // In this case, insert both particles anyway
          return;
        }


        // Keep subdividing
        this->subdivide(childIndex1);


        // Recalculate child indexes after further subdivisions
        childIndex1 = this->getNextNode(pos, childIndex1);
        childIndex2 = this->getNextNode(pos2, childIndex2);
      }

      // Once particles are in different children, insert them
      this->insertParticle(pos, mass, childIndex1);
      this->insertParticle(pos2, mass2, childIndex2);

      return;
    }

    // Case 3: The node is internal
    // Traverse the tree
    i = this->getNextNode(pos, i);
  }

}


/**
     * Get the next node based on the quadrant the particle is in
     * @param pos
     * @param i
     * @return
*/
int Octree::getNextNode(glm::vec4 pos, int i){
  Node &node = this->nodes[i];
  const glm::vec4 quadCenter = node.getQuadrantCenter();
  const int quadrantIndex = (pos.x < quadCenter.x) | (pos.y < quadCenter.y) << 1 | (pos.z < quadCenter.z) << 2;
  return node.getFirstChild() + quadrantIndex;
}

/**
 * Creates 8 new empty leaf nodes for the given node
 * @param i Node index
 */
void Octree::subdivide(int i){
  Node &node = this->nodes[i];

  // Set the first child to the current node count
  node.setFirstChild(this->nodeCount);

  // Mark this node as a parent
  this->parents[this->parentCount] = i;
  this->parentCount+=1;

  // Create the next child nodes
  this->nodeCount += 8;

  const glm::vec4 maxB = node.getMaxBoundary();
  const glm::vec4 minB = node.getMinBoundary();

  const float width = maxB.x - minB.x;
  const float height = maxB.y - minB.y;
  const float depth = maxB.z - minB.z;

  int currentChild = node.getFirstChild();
  int firstChild = currentChild;

#pragma unroll
  for (int j = 0; j < 8; ++j) {
    // Create the boundaries for the new node
    glm::vec4 minBoundary(0.f);
    glm::vec4 maxBoundary(0.f);

    // Check the 3rd bit
    if (j & 4) {
      minBoundary.z = minB.z;
      maxBoundary.z = minB.z + (depth/2.f);
    }
    else {
      minBoundary.z = minB.z + (depth/2.f);
      maxBoundary.z = maxB.z;
    }

    // Check the second bit
    if (j & 2) {
      minBoundary.y = minB.y;
      maxBoundary.y = minB.y + (height/2.f);
    }
    else {
      minBoundary.y = minB.y + (height/2.f);
      maxBoundary.y = maxB.y;
    }

    // Check the first bit
    if (j & 1) {
      minBoundary.x = minB.x;
      maxBoundary.x = minB.x + (width/2.f);
    }
    else {
      minBoundary.x = minB.x + (width/2.f);
      maxBoundary.x = maxB.x;
    }

    this->nodes[currentChild].createEmptyNode(minBoundary, maxBoundary);

    // Set the next node
    if (j < 7) {
      // The last child is different
      this->nodes[currentChild].setNext(firstChild+ j + 1);
    }

    // Next child node
    currentChild = firstChild + j + 1;
  }

  // The last child node points to the parent's next
  this->nodes[firstChild + 7].setNext(node.getNext());
}


/**
 * Once the particles are all inserted, propagate (bottom-up) the masses and centers of mass
 * O(p*8) where p is the number of parent nodes
 */
void Octree::propagate() {

  for (int i = parentCount - 1; i >= 0; i--) {
    // Compute the center of mass and mass of the current parent node
    int parentIndex = parents[i];
    glm::vec4 centerOfMass(0.f);
    glm::vec4 mass(0.f);

    Node &parent = this->nodes[parentIndex];

    #pragma unroll
    for (int j = 0; j < 8; ++j) {
      const int childIndex = parent.getFirstChild() + j;
      Node &child = this->nodes[childIndex];
      if (!child.isOccupied())  continue; // The leaf is empty
      centerOfMass += child.getCenterOfMass() * child.getMass();
      mass.x += child.getMass();
    }
    parent.setCenterOfMass(centerOfMass/mass.x);
    parent.setMass(mass);
  }
}

/**
 * Removes the empty nodes from the tree.
 * O(p*8) where p is the number of parent nodes
 */
void Octree::prune() {
  for (int i = 0; i < parentCount; i++) {
    int parentIndex = parents[i];
    Node &parent = this->nodes[parentIndex];
    if(!parent.isOccupied()){
      parent.setFirstChild(parent.getNext());
      continue;
    }
    int firstChild = -1;
    int lastChild = -1;


    for (int j = 0; j < 8; j++) {
      const int childIndex = parent.getFirstChild()+ j;

      if (this->nodes[childIndex].isOccupied()) {
        if (firstChild == -1) {
          firstChild = childIndex;
          lastChild = childIndex;
          continue;
        }
        this->nodes[lastChild].setNext(childIndex);
        lastChild = childIndex;
      }

    }

    this->nodes[lastChild].setNext(this->nodes[parentIndex].getNext());
    parent.setFirstChild(firstChild);
  }
}

/**
 * Computes the force for a single particle
 * @param pos
 * @param squaredSoftening
 * @param G
 * @return
 */
glm::vec4 Octree::computeGravityForce(glm::vec4& pos, const float squaredSoftening, const float G) {
  glm::vec4 force = glm::vec4(0.f);

  int i = 0; // Root of the tree

  while (i >= 0){
    Node &node = this->nodes[i];
    // We need to compute the size and the distance between the particle and the node
    glm::vec4 size = node.getMaxBoundary() - node.getMinBoundary();
    float s = glm::max(glm::max(size.x, size.y), size.z);
    float s_squared = s*s;

    glm::vec4 centerOfMass = node.getCenterOfMass();
    const glm::vec4 vector_i_j = centerOfMass - pos;
    const float dist_squared = glm::length2(vector_i_j);  // Squared distance

    // If the node is an occupied leaf OR satisfies the criteria
    if (node.isLeaf() || s_squared < this->theta * dist_squared) {
      // Compute the force
      if (node.isOccupied() && dist_squared > 0) {
        const float effective_dist_squared = dist_squared + squaredSoftening; // Apply softening to avoid 0 division
        const float inv_dist = 1.0f / std::pow(effective_dist_squared, 1.5f);
        force += ((vector_i_j * (G * node.getMass())) * inv_dist);
      }

      // Go to the next sibling or parent
      i = node.getNext();
    }
    else {
      // Go down the tree
      i = node.getFirstChild();
    }
  }

  return force;
}

/**
 * Inserts the particle into the node i
 * @param centerOfMass
 * @param mass
 * @param i
 */
void Octree::insertParticle(glm::vec4 centerOfMass, glm::vec4 mass, int i){
  Node &node = this->nodes[i];
  node.setCenterOfMass(centerOfMass);
  node.setMass(mass);
}

Node* Octree::getNodes() {
  return this->nodes;
}

int Octree::getMaxNodes() {
  return this->maxNodes;
}

Octree::~Octree(){
  //delete[] this->nodes;
  delete[] this->parents;
}

int Octree::getNodeCount() {
  return nodeCount;
}

void Octree::setNodes(Node *newNodes) {
  delete[] this->nodes;
  this->nodes = newNodes;
}
