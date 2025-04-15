#version 430 core

#define BLOCK_SIZE 1024

layout( local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1 ) in;

// Input particle positions
layout(std430, binding = 0) buffer PositionsBuffer {
    vec4 positions[]; 
};

// Output: The *first* intermediate buffer
layout(std430, binding = 6) buffer PartialBoundsBuffer { // Will be bound to partialA initially
    vec4 partialBounds[]; // Size: num_workgroups_pass1 * 2
};

// Uniform containing the total number of particles
uniform int numParticles;

// Shared memory for efficient reduction within the workgroup
// We store vec3 as we only care about coordinates for bounds
shared vec3 localMin[BLOCK_SIZE];
shared vec3 localMax[BLOCK_SIZE];

// Define large/small float constants 
const float LARGE_FLOAT =  3.402823466e+38F; // Max float
const float SMALL_FLOAT = -3.402823466e+38F; // Min float (most negative)

// --- 1. Initialize shared memory for this thread ---
void initSharedMemory(const uint localId, const uint globalId){
    // Each thread loads one particle's position (if it exists)
    if (globalId < numParticles) {
        localMin[localId] = positions[globalId].xyz;
        localMax[localId] = positions[globalId].xyz;
    } else {
        // Threads OUTSIDE the particle range initialize with identity values
        // Max possible float for min reduction, min possible float for max reduction
        localMin[localId] = vec3(LARGE_FLOAT);
        localMax[localId] = vec3(SMALL_FLOAT);
    }
    // Synchronize: Ensure all threads have loaded their initial values into shared memory
    // before starting the reduction. groupMemoryBarrier() ensures writes to shared memory
    // are visible to other threads in the workgroup.
    groupMemoryBarrier();
    barrier();
}

void performReduction(const uint localId, const uint workGroupSize){
     // --- 2. Perform reduction in shared memory ---
    // Explanation of the loop:
    // Example BLOCK_SIZE = 8
    // s = 4: threads 0,1,2,3 read from 0,1,2,3 and 4,5,6,7 respectively and combine. Results in localMin[0..3].
    // s = 2: threads 0,1 read from 0,1 and 2,3 respectively. Results in localMin[0..1].
    // s = 1: thread 0 reads from 0 and 1. Result in localMin[0].
    // At the end, localMin[0] and localMax[0] hold the min/max for the workgroup.
    for (uint s = workGroupSize / 2; s > 0; s >>= 1) { // s = BLOCK_SIZE/2, BLOCK_SIZE/4, ..., 1
        // Only the first 's' threads in the workgroup do the combining work
        if (localId < s) {
            localMin[localId] = min(localMin[localId], localMin[localId + s]);
            localMax[localId] = max(localMax[localId], localMax[localId + s]);
        }
        // Synchronize after each step to ensure writes from one step
        // are visible before the next step reads them.
        groupMemoryBarrier();
        barrier();
    }
}

void main() {
    const uint localId = gl_LocalInvocationID.x;    // Thread index within the workgroup (0 to BLOCK_SIZE-1)
    const uint globalId = gl_GlobalInvocationID.x; // Global thread index across all workgroups
    const uint workGroupSize = gl_WorkGroupSize.x;        // Workgroup size (BLOCK_SIZE)
    uint workGroupID = gl_WorkGroupID.x;
    
    initSharedMemory(localId, globalId);
    
    performReduction(localId, workGroupSize);
   
    
    // --- 3. Workgroup leader writes intermediate result ---
    if (localId == 0) {
        uint outputIndex = workGroupID * 2;
        partialBounds[outputIndex]   = vec4(localMin[0], 0.0); // Min
        partialBounds[outputIndex+1] = vec4(localMax[0], 0.0); // Max
    }
}