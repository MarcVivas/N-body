#include "Node.h"


/**
 * Node constructor
 */
Node::Node(){
  this->centerOfMass = glm::vec4(0.f);
  this->mass = glm::vec4(0.f);
  this->minBoundary = glm::vec4(0.f);
}


Node::~Node() {
  // Not freeing memory
}


