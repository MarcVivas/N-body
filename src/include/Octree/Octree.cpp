#include "Octree.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

Octree::Octree(int n){
  // TODO: Allocate memory for the array of nodes which will have a max capacity of approximately n log n
  this->maxNodes = std::floor(((std::ceil(n*std::log(n)) + 7 )/ 8)) * 8  * n;
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
  float min_y = std::numeric_limits<float>::max();
  float min_z = std::numeric_limits<float>::max();
  float max_x = std::numeric_limits<float>::min();
  float max_y = std::numeric_limits<float>::min();
  float max_z = std::numeric_limits<float>::min();

  // TODO: parallel loop
  for (int i = 0;  i < p->size(); ++i){
    if (p->getPositions()[i].x < min_x){
      min_x = p->getPositions()[i].x;
    }
    if (p->getPositions()[i].y < min_y){
      min_y = p->getPositions()[i].y;
    }
    if (p->getPositions()[i].z < min_z){
      min_z = p->getPositions()[i].z;
    }
    if (p->getPositions()[i].x > max_x){
      max_x = p->getPositions()[i].x;
    }
    if (p->getPositions()[i].y > max_y){
      max_y = p->getPositions()[i].y;
    }
    if (p->getPositions()[i].z > max_z){
      max_z = p->getPositions()[i].z;
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

    // Case 1: The node is an empty leaf
    if(this->nodes[i].isLeaf() && !this->nodes[i].isOccupied()){
      // Proceed to insert the particle
      this->insertParticle(pos, mass, i);
      return;
    }

    // Case 2: The node is an occupied leaf.
    if(this->nodes[i].isLeaf() && this->nodes[i].isOccupied()){
      // Get the current particle's position and mass
      glm::vec4 pos2 = this->nodes[i].getCenterOfMass();
      glm::vec4 mass2 = this->nodes[i].getMass();

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
  // Set the first child to the current node count
  this->nodes[i].setFirstChild(this->nodeCount);

  // Mark this node as a parent
  this->parents[this->parentCount] = i;
  this->parentCount+=1;

  // Create the next child nodes
  this->nodeCount += 8;


  const float width = this->nodes[i].getMaxBoundary().x - this->nodes[i].getMinBoundary().x;
  const float height = this->nodes[i].getMaxBoundary().y - this->nodes[i].getMinBoundary().y;
  const float depth = this->nodes[i].getMaxBoundary().z - this->nodes[i].getMinBoundary().z;

  int currentChild = this->nodes[i].getFirstChild();

  for (int j = 0; j < 8; ++j) {
    // Create a new child node
    glm::vec4 minBoundary(0.f);
    glm::vec4 maxBoundary(0.f);

    // Check the 3rd bit
    if (j >> 2 == 1) {
      minBoundary.z = this->nodes[i].getMinBoundary().z;
      maxBoundary.z = this->nodes[i].getMinBoundary().z + (depth/2.f);
    }
    else {
      minBoundary.z = this->nodes[i].getMinBoundary().z + (depth/2.f);
      maxBoundary.z = this->nodes[i].getMaxBoundary().z;
    }

    // Check the second bit
    if (j & (1 << 1)) {
      minBoundary.y = this->nodes[i].getMinBoundary().y;
      maxBoundary.y = this->nodes[i].getMinBoundary().y + (height/2.f);
    }
    else {
      minBoundary.y = this->nodes[i].getMinBoundary().y + (height/2.f);
      maxBoundary.y = this->nodes[i].getMaxBoundary().y;
    }

    // Check the first bit
    if (j & 1) {
      minBoundary.x = this->nodes[i].getMinBoundary().x;
      maxBoundary.x = this->nodes[i].getMinBoundary().x + (width/2.f);
    }
    else {
      minBoundary.x = this->nodes[i].getMinBoundary().x + (width/2.f);
      maxBoundary.x = this->nodes[i].getMaxBoundary().x;
    }
    this->nodes[currentChild].createEmptyNode(minBoundary, maxBoundary);

    // Set the next node
    this->nodes[currentChild].setNext(this->nodes[i].getFirstChild() + j + 1);

    // Next child node
    currentChild = this->nodes[i].getFirstChild() + j + 1;
  }

  // The last child node points to the parent's next
  this->nodes[this->nodes[i].getFirstChild() + 7].setNext(this->nodes[i].getNext());
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

    for (int j = 0; j < 8; ++j) {
      int childIndex = this->nodes[parentIndex].getFirstChild() + j;
      if (!this->nodes[childIndex].isOccupied())  continue; // The leaf is empty
      centerOfMass += this->nodes[childIndex].getCenterOfMass() * this->nodes[childIndex].getMass().x;
      mass.x += this->nodes[childIndex].getMass().x;
    }
    this->nodes[parentIndex].setCenterOfMass(centerOfMass/mass.x);
    this->nodes[parentIndex].setMass(mass);
  }


}

// Computes the force for a single particle
glm::vec4 Octree::computeGravityForce(glm::vec4 pos, const float squaredSoftening, const float G) {
  glm::vec4 force = glm::vec4(0.f);

  // Pre order iterative traversal (root, left, right)

  int i = 0; // Root of the tree
  do {
    // We need to compute the size and the distance between the particle and the node
    glm::vec4 size = this->nodes[i].getMaxBoundary() - this->nodes[i].getMinBoundary();
    float s_squared = std::pow(glm::max(glm::max(size.x, size.y), size.z), 2.0f);

    glm::vec4 centerOfMass = this->nodes[i].getCenterOfMass();
    const glm::vec4 vector_i_j = centerOfMass - pos;
    float dist = glm::length2(vector_i_j);


    // If the node is an occupied leaf OR satisfies the criteria
    if ((this->nodes[i].isLeaf() || s_squared < this->theta * dist)) {
      // Compute the force
      if (this->nodes[i].isOccupied() && dist > 0) {
        dist = dist + squaredSoftening;
        force += ((G * this->nodes[i].getMass().x) / dist) * vector_i_j;
      }


      if (this->nodes[i].getNext() == -1) break; // End

      // Go to the next sibling or parent
      i = this->nodes[i].getNext();
    }
    else {
      // Go down the tree
      i = this->nodes[i].getFirstChild();
    }
  } while (true);
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