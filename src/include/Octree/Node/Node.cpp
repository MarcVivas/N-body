#include "Node.h"


/**
 * Node constructor
 * @param capacity Max nodes of the tree
 */
Node::Node(int capacity){
  this->centerOfMass = new glm::vec4[capacity];
  this->mass = new glm::vec4[capacity];
  this->minBoundary = new glm::vec4[capacity];
}


Node::~Node() {
  // Not freeing memory because opengl does it for me
}


