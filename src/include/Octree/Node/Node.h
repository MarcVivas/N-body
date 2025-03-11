#include <glm/glm.hpp>
#ifndef NODE_H
#define NODE_H



struct alignas(16) Node {
  public:
    glm::vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
    glm::vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
    glm::vec4 minBoundary;  // maxBoundary.z = minBoundary.w

    Node();
    ~Node();

    void setFirstChild(int firstChild){
        this->mass.y = static_cast<float>(firstChild);
    }

    inline int getFirstChild() const{
        return static_cast<int>(this->mass.y);
    }

    void setNext(int next) {
        this->mass.z = static_cast<float>(next);
    }

    inline int getNext() const{
        return static_cast<int>(this->mass.z);
    }

    void setMass(glm::vec4 mass){
        this->mass.x = mass.x;
    }

    void setCenterOfMass(glm::vec4 centerOfMass){
        this->centerOfMass.x = centerOfMass.x;
        this->centerOfMass.y = centerOfMass.y;
        this->centerOfMass.z = centerOfMass.z;
    }


    inline void setBoundingBox(glm::vec4 minBoundary, glm::vec4 maxBoundary){
        this->minBoundary.x = minBoundary.x;
        this->minBoundary.y = minBoundary.y;
        this->minBoundary.z = minBoundary.z;
        this->mass.w = maxBoundary.x;
        this->centerOfMass.w = maxBoundary.y;
        this->minBoundary.w = maxBoundary.z;
    }


    inline void createEmptyNode(glm::vec4 minBoundary, glm::vec4 maxBoundary) {
        this->mass.x = -1.f;
        this->setFirstChild(-1);
        this->setBoundingBox(minBoundary, maxBoundary);
    }

    inline glm::vec4 getMinBoundary() const{
        glm::vec4 v = this->minBoundary;
        return {v.x, v.y, v.z, 0.f};
    }
    inline glm::vec4 getMaxBoundary() const{
        return {mass.w, centerOfMass.w, minBoundary.w, 0.f};
    }

    inline glm::vec4 getCenterOfMass() const {
        glm::vec4 c = centerOfMass;
        return {c.x, c.y, c.z, 0.f};
    }

    inline float getMass() const{
        return this->mass.x;
    }


    inline bool isLeaf() const{
        return getFirstChild() < 0;
    }

    inline bool isOccupied() const{
        return this->mass.x > 0.f;
    }

    inline glm::vec4 getQuadrantCenter() const {
        glm::vec4 maxBoundary = getMaxBoundary();
        glm::vec4 minBoundary = getMinBoundary();
        const float width = maxBoundary.x - minBoundary.x;
        const float height = maxBoundary.y - minBoundary.y;
        const float depth = maxBoundary.z - minBoundary.z;
        return {minBoundary.x + (width / 2.f), minBoundary.y + (height / 2.f), minBoundary.z + (depth / 2.f), 0.f};
    }
};



#endif //NODE_H
