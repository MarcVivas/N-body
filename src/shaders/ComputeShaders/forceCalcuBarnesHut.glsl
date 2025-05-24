#version 430 core

#define BLOCK_SIZE 256

layout(local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1) in;

uniform float squaredSoftening;
uniform float G;
uniform int numParticles;
uniform float theta;

struct Node {
    vec3 centerOfMass; // (x1*m1 + x2*m2) / (m1 + m2)
    float mass;
    vec3 minBoundary;
    uint numChildren;
    vec3 maxBoundary;
};

struct OctreeChildren {
    int children[8];
};

layout(std430, binding = 15) buffer octreeChildrenBuffer
{
    readonly OctreeChildren octreeChildren[];
};

layout(std430, binding = 0) buffer positionsBuffer
{
    readonly vec4 positions[];
};

layout(std430, binding = 4) buffer forcesBuffer
{
    writeonly vec4 forces[];
};

layout(std430, binding = 5) buffer nodesBuffer
{
    readonly Node nodes[];
};

#define STACK_SIZE 64

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= numParticles) return;

    vec3 force = vec3(0.f);

    int stack[STACK_SIZE];
    int stackTop = 0;
    stack[0] = 0;

    const vec3 particlePos = positions[index].xyz;

    while (stackTop >= 0 && stackTop < STACK_SIZE) {
        // Pop elem
        const int nodeId = stack[stackTop];
        stackTop--;

        const Node n = nodes[nodeId];

        // Try compute force with this Node
        const bool isLeaf = n.numChildren == 444;
        const vec3 size = n.maxBoundary.xyz - n.minBoundary.xyz;
        const float s = max(max(size.x, size.y), size.z);
        const float s_squared = s * s;
        const vec3 vec_i_j = n.centerOfMass.xyz - particlePos;
        const float squared_distance = dot(vec_i_j, vec_i_j);
        if (isLeaf || s_squared < theta * squared_distance && squared_distance > 0) {
            // Compute the force with this node
            const float effective_dist_squared = squared_distance + squaredSoftening;
            const float inv_sqrt_val = inversesqrt(effective_dist_squared);
            const float inv_dist = inv_sqrt_val * inv_sqrt_val * inv_sqrt_val;
            force += ((vec_i_j * (G * n.mass)) * inv_dist);
            continue;
        }

        // Go down the tree
        const OctreeChildren nodeChildren = octreeChildren[nodeId];

        for (uint i = 0; i < 8; i++) {
            const int childId = nodeChildren.children[i];
            if (childId < 0) continue;
            // Push childId
            stack[++stackTop] = childId;
        }
    }
    forces[index] = vec4(force, 0.f);
}
