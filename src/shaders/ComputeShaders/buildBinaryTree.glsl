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

layout(std430, binding = 8) buffer parentsBuffer
{
    int parents[];
};

layout(std430, binding = 9) buffer binaryTreeBuffer
{
    BinaryTree tree[];
};

layout(std430, binding = 13) buffer binaryTreeEdgesBuffer
{
    uint edges[]; // Every internal node owns 2 edges. Thus, internal node 0 has edges[0] and edges[0+1]
};

uniform int numInternalNodes;

uint countLeadingZeros(uint64_t x) {
    if (x == 0UL) return 64u;
    uint upper = uint(x >> 32);
    if (upper != 0u) {
        return (31u - findMSB(upper));
    } else {
        uint lower = uint(x & 0xFFFFFFFFUL);
        return 32u + (31u - findMSB(lower));
    }
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

int lenCommonPrefix(const uint64_t currentMorton, const int j) {
    return (j >= numInternalNodes + 1 || j < 0) ? -1 : int(countLeadingZeros(currentMorton ^ mortonCodes[j].mortonCode));
}

ivec4 determineRange(const int i, const uint64_t currentMorton) {
    // Determine the direction of the range (+1 or -1)
    // If direction = +1 then i + 1 belongs to the internal node i
    // If direction = -1 then i-1 belongs to the internal node i
    const int dir = lenCommonPrefix(currentMorton, i + 1) >= lenCommonPrefix(currentMorton, i - 1) ? 1 : -1;

    // Compute the upper bound of the range

    // Sibling node prefix length
    const int minPrefixLength = lenCommonPrefix(currentMorton, i - dir);
    int lmax = 2;

    // Expand until the condition is no longer satisfied in the direction that was determined before
    while (lenCommonPrefix(currentMorton, i + lmax * dir) > minPrefixLength) {
        lmax = lmax << 1; // Exponential increase: lmax * 2
    }

    // Find the end of the range with binary search
    int l = 0;

    // Find the exact position where the condition is no longer satisfied.
    // The elements inside the range [i, j] must satisfy the condition lenCommonPrefix(i, j) > minPrefixLength
    for (int t = lmax; t >= 1; t >>= 1) {
        if (lenCommonPrefix(currentMorton, i + (l + t) * dir) > minPrefixLength) {
            l = l + t;
        }
    }

    const int j = i + l * dir;

    return ivec4(i, j, dir, l);
}

int findSplit(const uint first, const uint last, const uint64_t firstMorton, const int dir, const int l) {
    const uint64_t lastMorton = mortonCodes[last].mortonCode;

    // Identical morton codes -> split at midpoint
    if (firstMorton == lastMorton)
        return int((first + last) >> 1); // (first + last) // 2

    // Example: firstMorton = 001; lastMorton = 010
    // 001 XOR 010 = 011
    // commonPrefix = countLeadingZeros(011);
    // commonPrefix = 1
    const uint commonPrefix = countLeadingZeros(firstMorton ^ lastMorton);

    int s = 0;
    int t = l;

    // Find the point where the condition is no longer satisfied.
    do {
        t = (t + 1) >> 1; // ceil(t/2)
        if (lenCommonPrefix(firstMorton, int(first) + (s + t) * dir) > commonPrefix) {
            s = s + t;
        }
    }
    while (t > 1);

    return int(first) + s * dir + min(dir, 0);
}

void main() {
    const int index = int(gl_GlobalInvocationID.x);
    if (index >= numInternalNodes) return;

    const uint64_t currentMorton = mortonCodes[index].mortonCode;

    const ivec4 range = determineRange(index, currentMorton);
    const int splitPos = findSplit(range.x, range.y, currentMorton, range.z, range.w);

    const int start = min(range.x, range.y);
    const int end = max(range.x, range.y);
    tree[index].start = start;
    tree[index].end = end;
    tree[index].split = splitPos;

    // Store how many octree nodes have to be created for each edge.
    const int offset = index << 1; // index * 2
    const uint64_t startRangeMorton = mortonCodes[start].mortonCode;
    const uint64_t endRangeMorton = mortonCodes[end].mortonCode;
    const int parentCommonPrefix = (index == 0) ? 0 : int(countLeadingZerosIgnoreFirst(startRangeMorton ^ endRangeMorton));
    const int leftChildCommonPrefix = int(countLeadingZerosIgnoreFirst(startRangeMorton ^ mortonCodes[splitPos].mortonCode));
    const int rightChildCommonPrefix = int(countLeadingZerosIgnoreFirst(endRangeMorton ^ mortonCodes[splitPos + 1].mortonCode));

    const int parentOctreeNodes = parentCommonPrefix / 3;

    // Left edge
    edges[offset] = min(1, (leftChildCommonPrefix / 3) - parentOctreeNodes);

    // Right edge
    edges[offset + 1] = min(1, (rightChildCommonPrefix / 3) - parentOctreeNodes);

    // For each node store the edge with its parent
    if (splitPos != start) parents[splitPos] = index * 2;
    if (splitPos + 1 != end) parents[splitPos + 1] = index * 2 + 1;
}
