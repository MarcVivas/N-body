#version 430 core

#define BLOCK_SIZE 1024

layout( local_size_x = BLOCK_SIZE, local_size_y = 1, local_size_z = 1 ) in;

// Input: Reads from the *current* source intermediate buffer
layout(std430, binding = 6) buffer InputBoundsBuffer { // Bound to partialA or partialB
    vec4 inputBounds[]; // Contains pairs of [min, max]
};

// Output: Writes reduced results to the *other* intermediate buffer
layout(std430, binding = 7) buffer OutputBoundsBuffer { // Bound to partialB or partialA
    vec4 outputBounds[]; // Writes pairs of [min, max]
};

// Uniform: Number of valid min/max pairs in the InputBoundsBuffer
uniform int numInputItems;

shared vec3 currentLocalMin[BLOCK_SIZE];
shared vec3 currentLocalMax[BLOCK_SIZE];

// Define large/small float constants 
const float LARGE_FLOAT =  3.402823466e+38F; // Max float
const float SMALL_FLOAT = -3.402823466e+38F; // Min float (most negative)

// --- 1. Initialize shared memory for this thread ---
void initSharedMemory(const uint readPairIndex, const uint readBufferIndex, const uint localId){

    if (readPairIndex < numInputItems) {
        currentLocalMin[localId] = inputBounds[readBufferIndex].xyz;     // Load min
        currentLocalMax[localId] = inputBounds[readBufferIndex + 1].xyz; // Load max
    } else {
        // Threads outside the valid range initialize with identity values
        currentLocalMin[localId] = vec3(LARGE_FLOAT);
        currentLocalMax[localId] = vec3(SMALL_FLOAT);
    }

    groupMemoryBarrier(); barrier(); // Ensure shared memory writes are complete
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
            currentLocalMin[localId] = min(currentLocalMin[localId], currentLocalMin[localId + s]);
            currentLocalMax[localId] = max(currentLocalMax[localId], currentLocalMax[localId + s]);
        }
        // Synchronize 
        groupMemoryBarrier(); barrier();    
    }
}

void main() {
    const uint localId = gl_LocalInvocationID.x;    // Thread index within the workgroup (0 to BLOCK_SIZE-1)
    const uint workGroupID = gl_WorkGroupID.x;
    const uint workGroupSize = gl_WorkGroupSize.x;        // Workgroup size (BLOCK_SIZE)

    // Calculate the index of the min/max pair this thread should read
    const uint readPairIndex = workGroupID * workGroupSize + localId;
    const uint readBufferIndex = readPairIndex * 2; // Index for min vec4 in inputBuffer


    initSharedMemory(readPairIndex, readBufferIndex, localId);
    
    performReduction(localId, workGroupSize);
   
    
    // --- 3. Workgroup leader (thread 0) writes the reduced result pair ---
    //    to the output buffer for the *next* pass.
    if (localId == 0 && readPairIndex < numInputItems) // Only write if this WG processed valid data
    {
        const uint outputIndex = workGroupID * 2; // Write at index corresponding to this WG's ID
        outputBounds[outputIndex]   = vec4(currentLocalMin[0], 0.0); // Min
        outputBounds[outputIndex+1] = vec4(currentLocalMax[0], 0.0); // Max
    }
}