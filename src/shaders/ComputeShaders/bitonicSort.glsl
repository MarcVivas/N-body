#version 430 core
// Required for uint64_t types and operations
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

uniform int numParticles;
uniform int k; // The size of the sub-sequences being merged/sorted (k in the algorithm)
uniform int j; // The comparison distance within the current pass (j in the algorithm)

void main() {
    uint index = gl_GlobalInvocationID.x;

    // Calculate the index of the element to compare with
    uint partnerIndex = index ^ j;

    if (partnerIndex < index || partnerIndex >= numParticles) return; // Partner is out of range or this thread shouldn't do the swap

    // Determine the sorting direction for this pair based on the larger block k
    // Elements in the first half of a block of size k sort ascending,
    // elements in the second half sort descending (within the bitonic merge step)
    const bool sortAscending = ((index & k) == 0);

    // Load the items (accessing the .x component for the key)
    const KeyIndex item1 = mortonCodes[index];
    const KeyIndex item2 = mortonCodes[partnerIndex];

    // Compare based on the Morton key (item.x)
    const bool shouldSwap = (item1.mortonCode > item2.mortonCode);

    // Perform swap if the order is wrong according to the sorting direction
    if (shouldSwap == sortAscending) {
        mortonCodes[index] = item2;
        mortonCodes[partnerIndex] = item1;
    }
}
