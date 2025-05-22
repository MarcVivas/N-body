#version 430 core

#define WG_SIZE 1024
#define ELEMENTS_PER_THREAD 2 // Must match Kernel 1
#define BLOCK_ELEMENTS (WG_SIZE * ELEMENTS_PER_THREAD)

layout(local_size_x = WG_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 13) buffer binaryTreeEdgesBuffer
{
    uint edges[]; // Every internal node owns 2 edges. Thus, internal node 0 has edges[0] and edges[0+1]
};

layout(std430, binding = 11) buffer BlockSumOutputBuffer {
    uint blockSums[];
};

uniform int N;

void main() {
    uint globalId = gl_GlobalInvocationID.x;

    if (globalId >= N) return; // Out of bounds

    const uint groupId = globalId / BLOCK_ELEMENTS;

    if (groupId == 0) return; // No need to compute the first block

    edges[globalId] += blockSums[groupId - 1];
}
