#include "Octree.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

/**
 * Constructor
 * @param n number of particles
 */
Octree::Octree(int n){
  this->maxNodes = n < 200 ? n*20 : n*8;
  this->nodes = new Node(maxNodes);
  this->parents = new int[this->maxNodes];
  std::cout <<"Max Nodes " <<  maxNodes << std::endl;
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
  this->nodes->setBoundingBox(glm::vec4(min_x, min_y, min_z, 0.f), glm::vec4(max_x, max_y, max_z, 0.f), 0);
}

/**
 * Clears the tree and recomputes the root's bounding box
 * O(n)
 * @param p Particles
 */
void Octree::reset(ParticleSystem* p){
  this->adjustBoundingBox(p);
  this->nodes->setFirstChild(-1, 0);
  this->nodes->setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f), 0);
  this->nodeCount = 1;
  this->parentCount = 0;
  this->nodes->setNext(-1, 0);
}


/**
 * Inserts a particle into the octree (iteratively and stackless)
 * Average O(n log(n))
 * @param pos Particle position
 * @param mass Particle mass
 */
void Octree::insert(glm::vec4 pos, glm::vec4 mass){
  // Start at the root of the octree
  int i = 0;

  while(i < this->maxNodes){

    // Case 1: The node is an empty leaf
    if(this->nodes->isLeaf(i) && !this->nodes->isOccupied(i)){
      // Proceed to insert the particle
      this->insertParticle(pos, mass, i);
      return;
    }

    // Case 2: The node is an occupied leaf.
    if(this->nodes->isLeaf(i) && this->nodes->isOccupied(i)){
      // Get the current particle's position and mass
      glm::vec4 pos2 = this->nodes->getCenterOfMass(i);
      glm::vec4 mass2 = glm::vec4(this->nodes->getMass(i), 0.f, 0.f, 0.f);

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
 * Creates 8 new empty leaf nodes for the given node
 * @param i Node index
 */
void Octree::subdivide(int i){

  // Set the first child to the current node count
  this->nodes->setFirstChild(this->nodeCount, i);

  // Mark this node as a parent
  this->parents[this->parentCount] = i;
  this->parentCount+=1;

  // Create the next child nodes
  this->nodeCount += 8;

  const glm::vec4 maxB = this->nodes->getMaxBoundary(i);
  const glm::vec4 minB = this->nodes->getMinBoundary(i);

  const float width = maxB.x - minB.x;
  const float height = maxB.y - minB.y;
  const float depth = maxB.z - minB.z;

  int currentChild = this->nodes->getFirstChild(i);
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

    this->nodes->createEmptyNode(minBoundary, maxBoundary, currentChild);

    // Set the next node
    if (j < 7) {
      // The last child is different
      this->nodes->setNext(firstChild+ j + 1, currentChild);
    }

    // Next child node
    currentChild = firstChild + j + 1;
  }

  // The last child node points to the parent's next
  this->nodes->setNext(this->nodes->getNext(i), firstChild+7);
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

    #pragma unroll
    for (int j = 0; j < 8; ++j) {
      const int childIndex = this->nodes->getFirstChild(parentIndex) + j;
      if (!this->nodes->isOccupied(childIndex))  continue; // The leaf is empty
      centerOfMass += this->nodes->getCenterOfMass(childIndex) * this->nodes->getMass(childIndex);
      mass.x += this->nodes->getMass(childIndex);
    }
    this->nodes->setCenterOfMass(centerOfMass/mass.x, parentIndex);
    this->nodes->setMass(mass, parentIndex);
  }
}

/**
 * Removes the empty nodes from the tree.
 * O(p*8) where p is the number of parent nodes
 */
void Octree::prune() {
  for (int i = 0; i < parentCount; i++) {
    int parentIndex = parents[i];
    int firstChild = -1;
    int lastChild = -1;
    for (int j = 0; j < 8; j++) {
      const int childIndex = this->nodes->getFirstChild(parentIndex)+ j;

      if (this->nodes->isOccupied(childIndex)) {
        if (firstChild == -1) {
          firstChild = childIndex;
          lastChild = childIndex;
          continue;
        }
        this->nodes->setNext(childIndex, lastChild);
        lastChild = childIndex;

      }

    }

    this->nodes->setNext(this->nodes->getNext(parentIndex), lastChild);
    this->nodes->setFirstChild(firstChild, parentIndex);
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


    // We need to compute the size and the distance between the particle and the node
    glm::vec4 size = this->nodes->getMaxBoundary(i) - this->nodes->getMinBoundary(i);
    float s = glm::max(glm::max(size.x, size.y), size.z);
    float s_squared = s*s;

    glm::vec4 centerOfMass = this->nodes->getCenterOfMass(i);
    const glm::vec4 vector_i_j = centerOfMass - pos;
    const float dist_squared = glm::length2(vector_i_j);  // Squared distance


    // If the node is an occupied leaf OR satisfies the criteria
    if (this->nodes->isLeaf(i) || s_squared < this->theta * dist_squared) {
      // Compute the force
      if (this->nodes->isOccupied(i) && dist_squared > 0) {
        const float effective_dist_squared = dist_squared + squaredSoftening; // Apply softening to avoid 0 division
        const float inv_dist = 1.0f / std::pow(effective_dist_squared, 1.5f);
        force += ((vector_i_j * (G * this->nodes->getMass(i))) * inv_dist);
      }

      // Go to the next sibling or parent
      i = this->nodes->getNext(i);
    }
    else {
      // Go down the tree
      i = this->nodes->getFirstChild(i);
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
  this->nodes->setCenterOfMass(centerOfMass, i);
  this->nodes->setMass(mass, i);
}

Node* Octree::getNodes() {
  return this->nodes;
}

int Octree::getMaxNodes() {
  return this->maxNodes;
}

Octree::~Octree(){
  delete this->nodes;
  delete[] this->parents;
}

int Octree::getNodeCount() {
  return nodeCount;
}
