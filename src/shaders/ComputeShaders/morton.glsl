#version 430 core

// Required for uint64_t types and operations
#extension GL_ARB_gpu_shader_int64 : require

#define BLOCK_SIZE 256

layout(local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer positionsBuffer
{
    vec4 positions[];
};

struct KeyIndex64 {
    uint64_t mortonCode;
    uint particleIndex;
};

layout(std430, binding = 16) buffer mortonBuffer
{
    KeyIndex64 mortonCodes[];
};

layout(std430, binding = 6) buffer InputBoundsBuffer {
    vec4 inputBounds[]; // inputBounds[0] = minBoundary; inputBounds[1] = maxBoundary
};

uniform int actualNumParticles;
uniform int paddedNumParticles;

// Constants for 64-bit Morton Code (21 bits per dimension)
#define MORTON_CODE_BITS_64 21
#define GRID_RESOLUTION_64 (1UL << MORTON_CODE_BITS_64) // 2^21 = 2,097,152. Use UL for uint64_t literal.
#define GRID_MASK_64 (GRID_RESOLUTION_64 - 1UL)     // 2,097,151, ( (1UL << 21) -1 )

// Expands a 21-bit integer 'v_in' into 63 bits by inserting two zeros
// between each bit. Output is uint64_t.
// This is the "part1by2" algorithm, using magic bitmasks.
// Based on the sequence often found in libmorton for Morton_SplitBy2 (uint32_t -> uint64_t)
// when input is masked to 21 bits.
uint64_t expandBits64(uint v_in) {
    uint64_t x = uint64_t(v_in); // Convert to uint64_t early
    x &= 0x1FFFFFUL; // Mask to ensure only 21 bits are used ( (1UL << 21) - 1UL )

    // The masks are designed to select the correct bits after they've been shifted
    // and spread apart. Each step handles progressively smaller groups of bits.
    // This sequence effectively takes an N-bit number and produces a 3N-bit number
    // by inserting two zeros between each original bit.
    // x = b20 b19 ... b0  -->  ... 00 b20 00 b19 ... 00 b0

    x = (x | (x << 32)) & 0x001F00000000FFFFUL; // Spreads the lower 16 bits and the next 5 bits (total 21 bits)
    // into positions ready for further spreading.
    // The 0xFFFFUL part handles original bits 0-15.
    // The 0x001F000000000000UL part handles original bits 16-20 (shifted by 32).
    // Result: b_20 .. b_16 are in bits 48-52 (approx), b_15 .. b_0 are in bits 0-15 (approx)
    // with large gaps between these two groups.

    x = (x | (x << 16)) & 0x001F0000FF0000FFUL; // Further spreads groups of 8 bits (from the low 16)
    // and groups of 2/3 bits (from the high 5).
    // The 0xFF sections handle 8-bit chunks.
    // The 0x1F section handles the 5-bit chunk.

    x = (x | (x << 8)) & 0x0100F00F00F00F00FUL; // Spreads groups of 4 bits.
    // The 0x0F sections handle 4-bit chunks.
    // The 0x01 section handles the highest remaining bit of the 5-bit chunk.

    x = (x | (x << 4)) & 0x010C30C30C30C30C3UL; // Spreads groups of 2 bits.
    // The 0xC3 sections (binary ..11000011) handle 2-bit chunks.
    // The 0x01 section handles the highest bit.

    x = (x | (x << 2)) & 0x01249249249249249UL; // Spreads individual bits to their final 3-bit slots (00b).
    // The 0x...49249... pattern (binary ...1001001...) selects bits for 00b form.
    // The 0x01 section handles the highest bit.
    return x;
}

// Calculates the 3D Morton code (63-bit) for a normalized position [0, 1]^3
uint64_t morton3D_64(vec3 normPos) {
    // Scale normalized coordinates to integer grid coordinates
    // Clamp to [0, GRID_MASK_64] which is [0, GRID_RESOLUTION_64 - 1]
    // Ensure input normPos components are [0,1]
    const float gridSizeF = float(GRID_RESOLUTION_64); // float(2^21)
    const float gridMaskF = float(GRID_MASK_64); // float(2^21 - 1)

    // It's crucial that normPos components are truly in [0,1].
    // Multiplying by GRID_RESOLUTION_64 and then flooring/truncating is typical.
    // Clamping to GRID_MASK_64 ensures we don't exceed the bit limit.
    const uint ix = uint(min(normPos.x * gridSizeF, gridMaskF));
    const uint iy = uint(min(normPos.y * gridSizeF, gridMaskF));
    const uint iz = uint(min(normPos.z * gridSizeF, gridMaskF));

    // Expand bits for each dimension
    const uint64_t ex = expandBits64(ix);
    const uint64_t ey = expandBits64(iy);
    const uint64_t ez = expandBits64(iz);

    // Interleave the expanded bits: Z-Y-X order (z gets lowest bits of each triplet)
    // Morton code: ... z2 y2 x2 z1 y1 x1 z0 y0 x0
    return (ex << 2) | (ey << 1) | ez;
}

// Function to normalize the position to the range [0, 1]
vec3 normalizedPos(vec3 pos, vec3 minBoundary, vec3 maxBoundary) {
    vec3 extent = maxBoundary - minBoundary;
    extent = max(extent, vec3(1e-6f)); // Avoid division by zero or very small extents

    // Normalize the position to the range [0, 1]
    vec3 norm = (pos - minBoundary) / extent;
    return clamp(norm, vec3(0.0f), vec3(1.0f)); // Ensure normalizedPos is within bounds [0, 1]
}

void main() {
    uint globalId_uint = gl_GlobalInvocationID.x;
    int globalId = int(globalId_uint); // Assuming paddedNumParticles fits in int for array indexing.

    if (globalId_uint >= uint(paddedNumParticles)) return;

    if (globalId_uint >= uint(actualNumParticles)) {
        mortonCodes[globalId].mortonCode = 0xFFFFFFFFFFFFFFFFUL; // Max uint64_t value for padding
        mortonCodes[globalId].particleIndex = 0xFFFFFFFFu; // Sentinel for padded particle index
        return;
    }

    const vec3 pos = positions[globalId].xyz;
    const vec3 minBoundary = inputBounds[0].xyz;
    const vec3 maxBoundary = inputBounds[1].xyz;

    const vec3 normPosVal = normalizedPos(pos, minBoundary, maxBoundary);

    mortonCodes[globalId].mortonCode = morton3D_64(normPosVal);
    mortonCodes[globalId].particleIndex = globalId_uint;
}
