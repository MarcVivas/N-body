#include "Node.h"

Node::Node(glm::vec4 centerOfMass, glm::vec4 mass){
  this->centerOfMass = centerOfMass;
  this->mass = mass;
  this->firstChild = -1;
  this->next = -1;
}

Node::Node(){
  this->firstChild = -1;
  this->centerOfMass = glm::vec4();
  this->mass = glm::vec4();
}



