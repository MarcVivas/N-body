#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

layout(std430, binding=0) buffer positionsBuffer
{
	vec4 positions[];
};

struct KeyIndex{
    uint mortonCode;
    uint particleIndex;
};

layout(std430, binding=16) buffer mortonBuffer
{
	KeyIndex mortonCodes[];    
};


layout(std430, binding = 6) buffer InputBoundsBuffer { // Bound to partialA or partialB
    vec4 inputBounds[]; // inputBounds[0] = minBoundary; inputBounds[1] = maxBoundary
};

uniform int actualNumParticles;
uniform int paddedNumParticles;

#define MORTON_CODE_BITS 10 // 10 bits for each dimension
#define GRID_RESOLUTION 1024 // 2^10
#define GRID_MASK (GRID_RESOLUTION - 1) // 1023

// Expands a 10-bit integer 'v' into 30 bits by inserting two zeros
// between each bit. e.g., abc -> 00a00b00c (for 3 bits)
// This is a common technique using "magic numbers"
uint expandBits(uint v){
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}

// Calculates the 3D Morton code for a normalized position [0, 1]^3
uint morton3D(vec3 normPos) {
    // Scale normalized coordinates to integer grid coordinates
    // Multiply by GRID_RESOLUTION - 1 for range [0, 1023] when MORTON_CODE_BITS = 10
    const uint ix = uint(min(max(normPos.x * float(GRID_RESOLUTION), 0.f), float(GRID_MASK)));
    const uint iy = uint(min(max(normPos.y * float(GRID_RESOLUTION), 0.f), float(GRID_MASK)));
    const uint iz = uint(min(max(normPos.z * float(GRID_RESOLUTION), 0.f), float(GRID_MASK)));

    // Expand bits for each dimension
    const uint ex = expandBits(ix);
    const uint ey = expandBits(iy);
    const uint ez = expandBits(iz);

    // Interleave the expanded bits: xxyyzz -> z..y..x..
    // x -> ..._x2_x1_x0
    // y -> ..._y2_y1_y0
    // z -> ..._z2_z1_z0
    // Interleaved: ... z2 y2 x2 z1 y1 x1 z0 y0 x0
    return ex * 4 + ey * 2 + ez;
}


// Function to normalize the position to the range [0, 1]
vec3 normalizedPos(vec3 pos, vec3 minBoundary, vec3 maxBoundary){
	vec3 extent = maxBoundary - minBoundary;
	extent = max(extent, vec3(1e-6)); // Avoid division by zero

	// Normalize the position to the range [0, 1]
	vec3 normalizedPos = (pos - minBoundary) / extent;
	return clamp(normalizedPos, vec3(0.0), vec3(1.0)); // Ensure normalizedPos is within bounds [0, 1]
}


void main(){
	int globalId = int(gl_GlobalInvocationID.x);

    if(globalId >= paddedNumParticles) return; 

    if(globalId >= actualNumParticles) {
        mortonCodes[globalId].mortonCode = 0xFFFFFFFFu; // Set Morton code to infinity
        return; // Skip threads that are not needed
    }	
	
    // Compute Morton code for the particle

	const vec3 pos = positions[globalId].xyz;
	const vec3 minBoundary = inputBounds[0].xyz;
	const vec3 maxBoundary = inputBounds[1].xyz;

	const vec3 normPos = normalizedPos(pos, minBoundary, maxBoundary);

	mortonCodes[globalId].mortonCode = morton3D(normPos);
    mortonCodes[globalId].particleIndex = uint(globalId);
}