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


    void setFirstChild(int firstChild){
        this->firstChild = firstChild;
    }

    void setMass(glm::vec4 mass){
        this->mass = mass;
    }

    void setCenterOfMass(glm::vec4 centerOfMass){
        this->centerOfMass = centerOfMass;
    }

    void setNext(int i) {
        this->next = i;
    }
    inline void setBoundingBox(glm::vec4 minBoundary, glm::vec4 maxBoundary){
        this->minBoundary = minBoundary;
        this->maxBoundary = maxBoundary;
    }


    inline void createEmptyNode(glm::vec4 minBoundary, glm::vec4 maxBoundary) {
        this->firstChild = -1;
        this->mass = glm::vec4(-1.f, 0.f, 0.f, 0.f);
        this->setBoundingBox(minBoundary, maxBoundary);
    }

    inline glm::vec4 getMinBoundary() const{
        return this->minBoundary;
    }
    inline glm::vec4 getMaxBoundary() const{
        return this->maxBoundary;
    }

    inline int getNext() const{
        return this->next;
    }

    inline glm::vec4 getCenterOfMass() const {
        return this->centerOfMass;
    }

    inline glm::vec4 getMass() const{
        return this->mass;
    }
    inline int getFirstChild() const{
        return this->firstChild;
    }

    inline bool isLeaf() const{
        return firstChild == -1;
    }

    inline bool isOccupied() const{
        return this->mass.x > 0.f;
    }

    inline glm::vec4 getQuadrantCenter() const {
        const float width = this->maxBoundary.x - this->minBoundary.x;
        const float height = this->maxBoundary.y - this->minBoundary.y;
        const float depth = this->maxBoundary.z - this->minBoundary.z;
        return {this->minBoundary.x + (width / 2.f), this->minBoundary.y + (height / 2.f), this->minBoundary.z + (depth / 2.f), 0.f};
    }
};



#endif //NODE_H
