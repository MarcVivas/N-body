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

void Node::setFirstChild(int firstChild){
  this->firstChild = firstChild;
}

void Node::setMass(glm::vec4 mass){
  this->mass = mass;
}

void Node::setCenterOfMass(glm::vec4 centerOfMass){
  this->centerOfMass = centerOfMass;
}

void Node::setBoundingBox(glm::vec4 minBoundary, glm::vec4 maxBoundary){
  this->minBoundary = minBoundary;
  this->maxBoundary = maxBoundary;
}

int Node::getNext() {
  return this->next;
}

glm::vec4 Node::getCenterOfMass(){
  return centerOfMass;
}
glm::vec4 Node::getMass(){
  return this->mass;
}
int Node::getFirstChild(){
  return this->firstChild;
}

bool Node::isLeaf(){
  return firstChild == -1;
}

bool Node::isOccupied(){
  return this->mass.x > 0.f;
}

glm::vec4 Node::getQuadrantCenter() const {
  const float width = this->maxBoundary.x - this->minBoundary.x;
  const float height = this->maxBoundary.y - this->minBoundary.y;
  const float depth = this->maxBoundary.z - this->minBoundary.z;
  return {this->minBoundary.x + (width / 2.f), this->minBoundary.y + (height / 2.f), this->minBoundary.z + (depth / 2.f), 0.f};
}

void Node::createEmptyNode(glm::vec4 minBoundary, glm::vec4 maxBoundary) {
  this->firstChild = -1;
  this->mass = glm::vec4(-1.f, 0.f, 0.f, 0.f);
  this->setBoundingBox(minBoundary, maxBoundary);
}

glm::vec4 Node::getMaxBoundary() {
  return this->maxBoundary;
}

void Node::setNext(int i) {
  this->next = i;
}

glm::vec4 Node::getMinBoundary() {
  return this->minBoundary;
}

