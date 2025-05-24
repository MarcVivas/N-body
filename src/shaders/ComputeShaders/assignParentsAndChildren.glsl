#version 430 core
#extension GL_ARB_gpu_shader_int64 : require

#define BLOCK_SIZE 256

layout(local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1) in;

struct OctreeChildren {
    int children[8];
};

layout(std430, binding = 15) buffer octreeChildrenBuffer
{
    OctreeChildren octreeChildren[];
};

struct KeyIndex {
    uint64_t mortonCode;
    uint particleIndex;
};

layout(std430, binding = 16) buffer mortonBuffer
{
    KeyIndex mortonCodes[];
};

struct BinaryTree {
    int start;
    int end;
    int split;
    // left = split
    // right = split+1
};

struct Node {
    vec3 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2)
    float mass;
    vec3 minBoundary;
    uint numChildren;
    vec3 maxBoundary;
};

layout(std430, binding = 5) buffer nodesBuffer
{
    Node nodes[];
};

layout(std430, binding = 9) buffer binaryTreeBuffer
{
    BinaryTree tree[];
};

// Prefix sum
layout(std430, binding = 13) buffer binaryTreeEdgesBuffer
{
    uint edges[]; // Every internal node owns 2 edges. Thus, internal node 0 has edges[0] and edges[0+1]
};

layout(std430, binding = 8) buffer binaryTreeParentsBuffer
{
    int binaryTreeParents[];
};

layout(std430, binding = 10) buffer octreeParentsBuffer
{
    int octreeParents[];
};

uniform int totalBinaryTreeEdges;

int getValFromPrefixSum(const int index) {
    return index < 0 ? 0 : int(edges[index]);
}

bool shouldLinkOctreeNode(const int currentVal, const int prevVal) {
    return bool(currentVal - prevVal);
}

uvec2 findOctreeParent(const int index) {
    uint currentBinaryNodeId = uint(index / 2); // 4

    int fatherEdgeId = binaryTreeParents[currentBinaryNodeId]; // 1
    uint fatherBinaryNodeId = uint(fatherEdgeId / 2); // 0
    int octreeId = getValFromPrefixSum(fatherEdgeId); // ocId
    int octreesCreatedByCurrentEdge = octreeId - getValFromPrefixSum(fatherEdgeId - 1); // 0

    // Skip edges with no octrees created
    while (octreesCreatedByCurrentEdge == 0 && fatherBinaryNodeId != 0) {
        currentBinaryNodeId = uint(fatherEdgeId / 2); // 7
        fatherEdgeId = binaryTreeParents[currentBinaryNodeId];
        fatherBinaryNodeId = uint(fatherEdgeId / 2);
        octreeId = getValFromPrefixSum(fatherEdgeId);
        octreesCreatedByCurrentEdge = octreeId - getValFromPrefixSum(fatherEdgeId - 1);
    }

    return fatherBinaryNodeId == 0 ? uvec2(0, 0) : uvec2(uint(octreeId), uint(currentBinaryNodeId));
}

uint countLeadingZerosIgnoreFirst(uint64_t x) {
    if (x == 0UL) return 63u; // Now max is 63 since we ignore the first bit
    uint upper = uint(x >> 32);
    if (upper != 0u) {
        return (30u - findMSB(upper)); // Adjusted from 31u to 30u
    } else {
        uint lower = uint(x & 0xFFFFFFFFUL);
        return 31u + (31u - findMSB(lower)); // First 31u adjusted from 32u
    }
}

uint getOctant(const int index, const uint binaryNodeParentId) {
    const int start = tree[binaryNodeParentId].start;
    const int end = tree[binaryNodeParentId].end;
    const uint commonPrefixLength = countLeadingZerosIgnoreFirst(mortonCodes[start].mortonCode ^ mortonCodes[end].mortonCode);
    const uint completeOctreeLevels = commonPrefixLength / 3;

    // Now get the bit position of the next uncompleted octree level
    const uint bitPosition = 62 - (completeOctreeLevels * 3) - 2; // 62 because the first bit is not used

    // Get the morton code
    const BinaryTree n = tree[uint(index / 2)];
    const uint64_t childMorton = bool(uint(index) & 1u) ? mortonCodes[n.split + 1].mortonCode : mortonCodes[n.split].mortonCode;
    return uint((childMorton >> bitPosition) & 0x7u);
}

/*
This compute shader links the octree nodes to their ancestors and children.
*/
void main() {
    const int index = int(gl_GlobalInvocationID.x);
    if (index >= totalBinaryTreeEdges) return;
    const int octreeNodeId = getValFromPrefixSum(index);
    if (!shouldLinkOctreeNode(octreeNodeId, getValFromPrefixSum(index - 1))) return;

    const uvec2 octreeParent = findOctreeParent(index);
    const int parentId = int(octreeParent.x);
    const uint binaryNodeParentId = octreeParent.y;

    octreeChildren[parentId].children[getOctant(index, binaryNodeParentId)] = octreeNodeId;
    octreeParents[octreeNodeId] = parentId;
    atomicAdd(nodes[parentId].numChildren, 1);
}
