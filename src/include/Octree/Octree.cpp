#include "Octree.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

Octree::Octree(int n){
  this->maxNodes = n < 200 ? n*20 : n*8;
  this->nodes = new Node[this->maxNodes];
  this->parents = new size_t[this->maxNodes];
  std::cout <<"Max Nodes " <<  maxNodes << std::endl;
  this->nodeCount = 0;
  this->parentCount = 0;
  this->theta = 0.5f *0.5f;
}

// Establish the bounding box of the first node
// O(n)
void Octree::adjustBoundingBox(ParticleSystem* p){
  float min_x = std::numeric_limits<float>::max();
  float min_y = min_x;
  float min_z = min_x;
  float max_x = std::numeric_limits<float>::min();
  float max_y = max_x;
  float max_z = max_x;

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

  this->nodes[0].setBoundingBox(glm::vec4(min_x, min_y, min_z, 0.f), glm::vec4(max_x, max_y, max_z, 0.f));
}

// Clear the tree and recompute the bounding box
// O(n)
void Octree::reset(ParticleSystem* p){
  this->adjustBoundingBox(p);
  this->nodes[0].setFirstChild(-1);
  this->nodes[0].setMass(glm::vec4(-1.f, 0.f, 0.f, 0.f));
  this->nodeCount = 1;
  this->parentCount = 0;
  this->nodes[0].setNext(-1);
}


// Inserts a particle to the octree (iteratively)
// O(n logBase8(n))
void Octree::insert(glm::vec4 pos, glm::vec4 mass){
  // Start at the root of the octree
  size_t i = 0;



  while(i < this->maxNodes){
    Node& currentNode = this->nodes[i];

    // Case 1: The node is an empty leaf
    if(currentNode.isLeaf() && !currentNode.isOccupied()){
      // Proceed to insert the particle
      this->insertParticle(pos, mass, i);
      return;
    }

    // Case 2: The node is an occupied leaf.
    if(currentNode.isLeaf() && currentNode.isOccupied()){
      // Get the current particle's position and mass
      glm::vec4 pos2 = currentNode.getCenterOfMass();
      glm::vec4 mass2 = currentNode.getMass();

      // Subdivide the current node
      this->subdivide(i);

      // Get child indexes
      size_t childIndex1 = this->getNextNode(pos, i);
      size_t childIndex2 = this->getNextNode(pos2, i);
      const float dist = glm::length(pos2-pos);

      // If both particles go to the same child node we need to keep subdividing
      while (childIndex1 == childIndex2) {
        // Edge case: Both particles are in the same position or very close
        if (dist < 1e-6) {
          this->insertParticle(pos, mass + mass, childIndex1);  // In this case, insert both particles anyway
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


// Create 8 new empty leaf nodes for the current node
void Octree::subdivide(size_t i){
  const Node& parent = this->nodes[i];
  // Set the first child to the current node count
  this->nodes[i].setFirstChild(this->nodeCount);

  // Mark this node as a parent
  this->parents[this->parentCount] = i;
  this->parentCount+=1;

  // Create the next child nodes
  this->nodeCount += 8;

  const glm::vec4 maxB = parent.getMaxBoundary();
  const glm::vec4 minB = parent.getMinBoundary();

  const float width = maxB.x - minB.x;
  const float height = maxB.y - minB.y;
  const float depth = maxB.z - minB.z;

  int currentChild = parent.getFirstChild();
  int firstChild = currentChild;

#pragma unroll
  for (int j = 0; j < 8; ++j) {
    // Create a new child node
    glm::vec4 minBoundary(0.f);
    glm::vec4 maxBoundary(0.f);

    // Check the 3rd bit
    if (j >> 2 == 1) {
      minBoundary.z = minB.z;
      maxBoundary.z = minB.z + (depth/2.f);
    }
    else {
      minBoundary.z = minB.z + (depth/2.f);
      maxBoundary.z = minB.z;
    }

    // Check the second bit
    if (j & (1 << 1)) {
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
    this->nodes[currentChild].setNext(firstChild+ j + 1);

    // Next child node
    currentChild = firstChild + j + 1;
  }

  // The last child node points to the parent's next
  this->nodes[firstChild+ 7].setNext(parent.getNext());
}

// Get the next node based on the quadrant the particle is in
size_t Octree::getNextNode(glm::vec4 pos, size_t i){
  const glm::vec4 quadCenter = this->nodes[i].getQuadrantCenter();
  const int quadrantIndex = (pos.x < quadCenter.x) | (pos.y < quadCenter.y) << 1 | (pos.z < quadCenter.z) << 2;
  return this->nodes[i].getFirstChild() + quadrantIndex;
}

// Once the particles are all inserted, propagate (bottom-up) the masses and centers of mass
void Octree::propagate() {

  for (int i = parentCount - 1; i >= 0; i--) {
    // Compute the center of mass and mass of the current parent node
    size_t parentIndex = parents[i];
    glm::vec4 centerOfMass(0.f);
    glm::vec4 mass(0.f);

    Node& parentNode = this->nodes[parentIndex];

    #pragma unroll
    for (int j = 0; j < 8; ++j) {
      const int childIndex = parentNode.getFirstChild() + j;
      Node& childNode = this->nodes[childIndex];
      if (!childNode.isOccupied())  continue; // The leaf is empty
      centerOfMass += childNode.getCenterOfMass() * childNode.getMass().x;
      mass.x += childNode.getMass().x;
    }
    parentNode.setCenterOfMass(centerOfMass/mass.x);
    parentNode.setMass(mass);
  }

}

// Computes the force for a single particle
glm::vec4 Octree::computeGravityForce(glm::vec4& pos, const float squaredSoftening, const float G) {
  glm::vec4 force = glm::vec4(0.f);

  int i = 0; // Root of the tree

  while (true){
    // Get node reference
    Node &currentNode = this->nodes[i];

    // We need to compute the size and the distance between the particle and the node
    glm::vec4 size = currentNode.getMaxBoundary() - currentNode.getMinBoundary();
    float s = glm::max(glm::max(size.x, size.y), size.z);
    float s_squared = s*s;

    glm::vec4 centerOfMass = currentNode.getCenterOfMass();
    const glm::vec4 vector_i_j = centerOfMass - pos;
    const float dist_squared = glm::length2(vector_i_j);  // Squared distance


    // If the node is an occupied leaf OR satisfies the criteria
    if (currentNode.isLeaf() || s_squared < this->theta * dist_squared) {
      // Compute the force
      if (currentNode.isOccupied() && dist_squared > 0) {
        const float effective_dist_squared = dist_squared + squaredSoftening; // Apply softening to avoid 0 division
        const float inv_dist = 1.0f / effective_dist_squared;
        force += ((vector_i_j * (G * currentNode.getMass().x)) * inv_dist);
      }


      if (currentNode.getNext() == -1) break; // End, exit the loop

      // Go to the next sibling or parent
      i = currentNode.getNext();
    }
    else {
      // Go down the tree
      i = currentNode.getFirstChild();
    }
  }

  return force;
}

void Octree::insertParticle(glm::vec4 centerOfMass, glm::vec4 mass, size_t i){
  this->nodes[i].setCenterOfMass(centerOfMass);
  this->nodes[i].setMass(mass);
}

Octree::~Octree(){
  delete[] this->nodes;
  delete[] this->parents;
}