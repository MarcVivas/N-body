#version 430 core
#extension GL_ARB_gpu_shader_int64 : require

#define BLOCK_SIZE 256
#define FLOAT_MIN 1.175494351e-38
#define FLOAT_MAX 3.402823466e+38

layout(local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1) in;

struct Node {
    vec3 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2)
    float mass;
    vec3 minBoundary;
    uint numChildren;
    vec3 maxBoundary;
    float padding2;
    int children[8];
};

layout(std430, binding = 5) buffer nodesBuffer
{
    Node nodes[];
};

// Prefix sum
layout(std430, binding = 13) buffer binaryTreeEdgesBuffer
{
    uint edges[]; // Every internal node owns 2 edges. Thus, internal node 0 has edges[0] and edges[0+1]
};

layout(std430, binding = 10) buffer octreeParentsBuffer
{
    int octreeParents[];
};

uniform int totalBinaryTreeEdges;

int getTotalOctreeNodes() {
    // Octree nodes created + root
    return int(edges[totalBinaryTreeEdges - 1]) + 1;
}

/*
This compute shader propagates the mass, center of mass, min and max boundaries of the leaf nodes to the top. (In a single pass)
*/
void main() {
    const int index = int(gl_GlobalInvocationID.x);
    if (index >= getTotalOctreeNodes()) return;

    int currentOctreeId = index;
    if (nodes[currentOctreeId].numChildren > 0) return;
    // Only leafs now
    while (currentOctreeId != 0) { // While not root
        int parent = octreeParents[currentOctreeId];
        // Notify parent
        uint val = atomicAdd(nodes[parent].numChildren, -1);
        if (val > 1) return;
        // val = 0 -> All children have notified the ancestor
        // This thread becomes the parent
        currentOctreeId = parent;
        Node p = nodes[currentOctreeId];
        float mass = 0.f;
        vec3 centerOfMass = vec3(0.f);
        vec3 minBoundary = vec3(FLOAT_MAX);
        vec3 maxBoundary = vec3(FLOAT_MIN);
        for (uint i = 0; i < 8; i++) {
            const int child = p.children[i];
            if (child < 0) continue; // Child does not exist
            // Child exists
            Node c = nodes[child];
            mass += c.mass;
            centerOfMass += c.centerOfMass * c.mass;
            minBoundary.x = min(minBoundary.x, c.minBoundary.x);
            minBoundary.y = min(minBoundary.y, c.minBoundary.y);
            minBoundary.z = min(minBoundary.z, c.minBoundary.z);
            maxBoundary.x = max(maxBoundary.x, c.maxBoundary.x);
            maxBoundary.y = max(maxBoundary.y, c.maxBoundary.y);
            maxBoundary.z = max(maxBoundary.z, c.maxBoundary.z);
        }
        nodes[currentOctreeId].centerOfMass = centerOfMass / mass;
        nodes[currentOctreeId].mass = mass;
        nodes[currentOctreeId].minBoundary = minBoundary;
        nodes[currentOctreeId].maxBoundary = maxBoundary;
    }
}
