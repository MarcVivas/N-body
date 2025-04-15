#include "Node.h"
#include "Helpers.h"

using namespace helpers;
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

