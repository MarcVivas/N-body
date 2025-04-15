#version 430 core

#define BLOCK_SIZE 64

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

struct Node{
    vec4 mass; // mass = mass.x; firstChild=mass.y; next=mass.z; maxBoundary.x=mass.w
    vec4 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2); maxBoundary.y = centerOfMass.w
    vec4 minBoundary;  // maxBoundary.z = minBoundary.w
};

// Input: Reads from the intermediate buffer
layout(std430, binding = 6) buffer InputBoundsBuffer { // Bound to partialA or partialB
    vec4 inputBounds[]; // Contains pairs of [min, max]
};

layout(std430, binding=5) buffer nodesBuffer
{
    Node nodes[];
};

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

void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index == 0){
        setMinBoundary(inputBounds[0], 0);
        setMaxBoundary(inputBounds[1], 0);
        setFirstChild(-1.0f, 0);
        setMass(-1.0f, 0);
        setNext(-1.0f, 0);

    }

}