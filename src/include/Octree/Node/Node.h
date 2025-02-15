#include <glm/glm.hpp>
#ifndef NODE_H
#define NODE_H



class Node {
  public:
    Node();
    Node(glm::vec4 centerOfMass, glm::vec4 mass);
    glm::vec4 mass;
    glm::vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2)
    glm::vec4 maxBoundary;
    glm::vec4 minBoundary;
    int firstChild;
    int next;
    void setNext(int i);
    int getNext();
    void setBoundingBox(glm::vec4 minBoundary, glm::vec4 maxBoundary);
    void setFirstChild(int firstChild);
    void setMass(glm::vec4 mass);
    void setCenterOfMass(glm::vec4 centerOfMass);
    glm::vec4 getCenterOfMass();
    glm::vec4 getMass();
    int getFirstChild();
    bool isLeaf();
    bool isOccupied();
    glm::vec4 getQuadrantCenter() const;
    void createEmptyNode(glm::vec4 minBoundary, glm::vec4 maxBoundary);
    glm::vec4 getMinBoundary();
    glm::vec4 getMaxBoundary();
};



#endif //NODE_H
