#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;


struct Node{
    vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
    vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
    vec4 minBoundary;  // maxBoundary.z = minBoundary.w
};

layout(std430, binding=5) buffer nodesBuffer
{
    Node nodes[];
};

layout(std430, binding=8) buffer parentsBuffer
{
    int parents[];
};

uniform int currentNodeCount;
uniform int nextLevelNode;

void setMinBoundary(const vec4 b, const uint i){
    nodes[i].minBoundary.xyz = b.xyz;
}

void setMaxBoundary(const vec4 b, const uint i){
    nodes[i].mass.w = b.x;
    nodes[i].centerOfMass.w = b.y;
    nodes[i].minBoundary.w = b.z; 
}

void setFirstChild(const float firstChild, const uint i){
    nodes[i].mass.y = firstChild;
}

void setMass(const float mass, const uint i){
    nodes[i].mass.x = mass;
}

void setNext(const float next, const uint i){
    nodes[i].mass.z = next; 
}

vec4 getMinBoundary(const uint i){
    return vec4(nodes[i].minBoundary.xyz, 0);
}

vec4 getMaxBoundary(const uint i){
    return vec4(nodes[i].mass.w, nodes[i].centerOfMass.w, nodes[i].minBoundary.w, 0.f);
}

uint getFirstChild(const uint i){
    return uint(nodes[i].mass.y);
}

uint getNext(const uint i){
    return uint(nodes[i].mass.z);
}

void createEmptyNode(const uint i, const vec4 minB, const vec4 maxB){
    setMinBoundary(minB, i);
    setMaxBoundary(maxB, i);
    setFirstChild(-1.0f, i);
    setMass(-1.0f, i);
}

void subdivide(const uint i){
    
  // Set the first child to the current node count
  
  setFirstChild(8*gl_GlobalInvocationID.x+nextLevelNode, i);
  

  // All the nodes in this shader will be parents
  // Mark this node as a parent
  parents[i] = int(i);
  
 


  // Create the next child nodes


  const vec4 maxB = getMaxBoundary(i);
  const vec4 minB = getMinBoundary(i);

  const float width = maxB.x - minB.x;
  const float height = maxB.y - minB.y;
  const float depth = maxB.z - minB.z;

  uint currentChild = getFirstChild(i);
  uint firstChild = currentChild;

  for (int j = 0; j < 8; ++j) {
    // Create the boundaries for the new node
    vec4 minBoundary = vec4(0.f);
    vec4 maxBoundary = vec4(0.f);

    // Check the 3rd bit
    if ((j & 4) != 0) {
      minBoundary.z = minB.z;
      maxBoundary.z = minB.z + (depth/2.f);
    }
    else {
      minBoundary.z = minB.z + (depth/2.f);
      maxBoundary.z = maxB.z;
    }

    // Check the second bit
    if ((j & 2) != 0) {
      minBoundary.y = minB.y;
      maxBoundary.y = minB.y + (height/2.f);
    }
    else {
      minBoundary.y = minB.y + (height/2.f);
      maxBoundary.y = maxB.y;
    }

    // Check the first bit
    if ((j & 1) != 0) {
      minBoundary.x = minB.x;
      maxBoundary.x = minB.x + (width/2.f);
    }
    else {
      minBoundary.x = minB.x + (width/2.f);
      maxBoundary.x = maxB.x;
    }

    createEmptyNode(currentChild, minBoundary, maxBoundary);

    // Set the next node
    if (j < 7) {
      // The last child is different
      setNext(firstChild + j + 1, currentChild);
    }

    // Next child node
    currentChild = firstChild + j + 1;
  }

  // The last child node points to the parent's next
  setNext(getNext(i), firstChild + 7);
}


void main(){
    const uint globalThreadId = gl_GlobalInvocationID.x;
    if (globalThreadId >= nextLevelNode-currentNodeCount){
        return; // Not you
    }

    subdivide(currentNodeCount+globalThreadId);
}
