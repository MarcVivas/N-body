#include <glm/glm.hpp>
#ifndef NODE_H
#define NODE_H



struct alignas(16) Node {
  public:
    glm::vec4* mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
    glm::vec4* centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
    glm::vec4* minBoundary;  // maxBoundary.z = minBoundary.w

    Node(int capacity);
    ~Node();

    void setFirstChild(int firstChild, int i){
        this->mass[i].y = static_cast<float>(firstChild);
    }

    inline int getFirstChild(int i) const{
        return static_cast<int>(this->mass[i].y);
    }

    void setNext(int next, int node) {
        this->mass[node].z = static_cast<float>(next);
    }

    inline int getNext(int node) const{
        return static_cast<int>(this->mass[node].z);
    }

    void setMass(glm::vec4 mass, int node){
        this->mass[node].x = mass.x;
    }

    void setCenterOfMass(glm::vec4 centerOfMass, int node){
        this->centerOfMass[node].x = centerOfMass.x;
        this->centerOfMass[node].y = centerOfMass.y;
        this->centerOfMass[node].z = centerOfMass.z;
    }


    inline void setBoundingBox(glm::vec4 minBoundary, glm::vec4 maxBoundary, int node){
        this->minBoundary[node].x = minBoundary.x;
        this->minBoundary[node].y = minBoundary.y;
        this->minBoundary[node].z = minBoundary.z;
        this->mass[node].w = maxBoundary.x;
        this->centerOfMass[node].w = maxBoundary.y;
        this->minBoundary[node].w = maxBoundary.z;
    }


    inline void createEmptyNode(glm::vec4 minBoundary, glm::vec4 maxBoundary, int node) {
        this->mass[node].x = -1.f;
        this->setFirstChild(-1, node);
        this->setBoundingBox(minBoundary, maxBoundary, node);
    }

    inline glm::vec4 getMinBoundary(int node) const{
        glm::vec4 v = this->minBoundary[node];
        return {v.x, v.y, v.z, 0.f};
    }
    inline glm::vec4 getMaxBoundary(int node) const{
        return {mass[node].w, centerOfMass[node].w, minBoundary[node].w, 0.f};
    }

    inline glm::vec4 getCenterOfMass(int node) const {
        glm::vec4 c = centerOfMass[node];
        return {c.x, c.y, c.z, 0.f};
    }

    inline float getMass(int node) const{
        return this->mass[node].x;
    }


    inline bool isLeaf(int node) const{
        return getFirstChild(node) < 0;
    }

    inline bool isOccupied(int node) const{
        return this->mass[node].x > 0.f;
    }

    inline glm::vec4 getQuadrantCenter(int node) const {
        glm::vec4 maxBoundary = getMaxBoundary(node);
        glm::vec4 minBoundary = getMinBoundary(node);
        const float width = maxBoundary.x - minBoundary.x;
        const float height = maxBoundary.y - minBoundary.y;
        const float depth = maxBoundary.z - minBoundary.z;
        return {minBoundary.x + (width / 2.f), minBoundary.y + (height / 2.f), minBoundary.z + (depth / 2.f), 0.f};
    }

    void setMasses(glm::vec4* ptr) {
        delete[] this->mass;
        this->mass = ptr;
    }

    void setCenterOfMasses(glm::vec4* ptr) {
        delete[] this->centerOfMass;
        this->centerOfMass = ptr;
    }

    void setMinBoundaries(glm::vec4* ptr) {
        delete[] this->minBoundary;
        this->minBoundary = ptr;
    }
};



#endif //NODE_H
