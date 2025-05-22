#version 430 core
#extension GL_ARB_gpu_shader_int64 : require

#define BLOCK_SIZE 256

layout(local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1) in;

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
    float padding2;
    int children[8];
};

layout(std430, binding = 5) buffer nodesBuffer
{
    Node nodes[];
};

layout(std430, binding = 0) buffer positionsBuffer
{
    readonly vec4 positions[];
};

layout(std430, binding = 3) buffer massesBuffer
{
    readonly vec4 masses[];
};

layout(std430, binding = 9) buffer binaryTreeBuffer
{
    BinaryTree tree[];
};

layout(std430, binding = 13) buffer binaryTreeEdgesBuffer
{
    uint edges[]; // Every internal node owns 2 edges. Thus, internal node 0 has edges[0] and edges[0+1]
};

uniform int totalBinaryTreeEdges;

int getValFromPrefixSum(const int index) {
    return index < 0 ? 0 : int(edges[index]);
}

bool shouldCreateOctreeNode(const int currentVal, const int prevVal) {
    return bool(currentVal - prevVal);
}

bool isLeaf(const BinaryTree binaryTreeNode, const int edgeIndex) {
    const bool isOdd = bool(edgeIndex & 1);
    // If it's odd, the edge is the right child of its parent
    // If it's even, the edge is the left child of its parent
    return isOdd ? binaryTreeNode.split + 1 == binaryTreeNode.end : binaryTreeNode.split == binaryTreeNode.start;
}

Node createEmptyOctreeNode() {
    return Node(vec3(0), 0.f, vec3(0), 0u, vec3(0), 0.f,
        int[8](
            -1, -1, -1, -1, -1, -1, -1, -1
        ));
}

Node createLeafOctreeNode(const BinaryTree binaryTreeNode, const int edgeIndex) {
    const bool isOdd = bool(edgeIndex & 1);
    const int mortonCodeId = isOdd ? binaryTreeNode.end : binaryTreeNode.start;
    const uint particleId = mortonCodes[mortonCodeId].particleIndex;
    const vec3 position = positions[particleId].xyz;
    const float mass = masses[particleId].x;
    return Node(position, mass, position, 0u, position, 0.f,
        int[8](
            -1, -1, -1, -1, -1, -1, -1, -1
        ));
}

/*
This compute shader creates the nodes for the octree using the prefix sum of the binary tree edges.
The internal nodes will be empty and filled in the next shader.
The leaf nodes are filled in this shader.
*/
void main() {
    const int index = int(gl_GlobalInvocationID.x);
    if (index >= totalBinaryTreeEdges) return;
    const int octreeNodeId = getValFromPrefixSum(index);
    if (index == 0) {
        nodes[0] = createEmptyOctreeNode();
    }
    if (!shouldCreateOctreeNode(octreeNodeId, getValFromPrefixSum(index - 1))) return;

    // Create the octree node.
    // In case of a leaf node, initialize it with its mass and center of mass
    // In case of an internal node, initialize it with its default values
    const BinaryTree binaryTreeNode = tree[uint(index / 2)];
    nodes[octreeNodeId] = isLeaf(binaryTreeNode, index) ? createLeafOctreeNode(binaryTreeNode, index) : createEmptyOctreeNode();
}
