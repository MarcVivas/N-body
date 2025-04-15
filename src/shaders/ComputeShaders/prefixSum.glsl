#version 430 core

#define WG_SIZE 1024 
#define ELEMENTS_PER_THREAD 2
#define BLOCK_ELEMENTS (WG_SIZE * ELEMENTS_PER_THREAD)


layout( local_size_x = WG_SIZE, local_size_y =1, local_size_z = 1  ) in;


struct Task {
    int root; 
    int totalParticles;
};

// input
layout(std430, binding=9) buffer tasksBuffer
{
    Task tasks[];
};

// Output for the sum of each block
layout(std430, binding=11) buffer BlockSumOutputBuffer {
    writeonly int blockSums[];
};


uniform int N;
uniform bool secondPass;
shared int sharedTotalParticlesPerTask[BLOCK_ELEMENTS];

// --- Helper ---
int getTotalParticles(uint index) {
    if (index < N && !secondPass) {
        // Only read the relevant field
        return tasks[index].totalParticles;
    }
    else if(index < N && secondPass){
        return blockSums[index];
    } 
    else {
        return 0; // Identity element for addition
    }
}



// Remeber each thread processes 2 elements
// Each work group processes 2 * WG_SIZE elements
void main(){
    uint localId   = gl_LocalInvocationID.x; // Thread ID within WG (0..WG_SIZE-1)
    uint globalId  = gl_GlobalInvocationID.x; // Global thread ID (across all WGs)
    uint groupId   = gl_WorkGroupID.x;       // Workgroup ID


    // Load into shared memory the data this thread will be using 
    const int val1 = getTotalParticles(groupId * BLOCK_ELEMENTS + localId);
    const int val2 = getTotalParticles(groupId * BLOCK_ELEMENTS + localId + WG_SIZE);

    sharedTotalParticlesPerTask[localId] = val1;
    sharedTotalParticlesPerTask[localId + WG_SIZE] = val2;

    groupMemoryBarrier(); barrier(); // Ensure shared memory writes are complete

    // Up-sweep phase (reduction)
    // Builds partial sums. The last element shared_data[BLOCK_ELEMENTS-1] will hold the block sum (total sum of the block).
    uint stride = 1; 
    for (uint depth = 0; depth < log2(BLOCK_ELEMENTS); depth++){
        stride = 1 << depth;    // This is equal to stride = 2^depth
        
        uint writeIndex = (localId + 1) * (stride*2) - 1; // Only odd indexes
        uint readIndex = writeIndex - stride; 

        // Check if the write index is within the block
        if (writeIndex < BLOCK_ELEMENTS){
            sharedTotalParticlesPerTask[writeIndex] += sharedTotalParticlesPerTask[readIndex];
        }

        groupMemoryBarrier(); barrier(); // Ensure shared memory writes are complete
    }

    // The last element of shared memory contains the sum of the block
    // Only the last thread stores the value in the blockSums buffer
    if (localId == WG_SIZE-1){
        blockSums[groupId] = sharedTotalParticlesPerTask[BLOCK_ELEMENTS - 1];
        sharedTotalParticlesPerTask[BLOCK_ELEMENTS - 1] = 0; // Reset for the down-sweep phase
    }

    groupMemoryBarrier(); barrier(); // Ensure shared memory writes are complete


    // Down-sweep phase (distribution) Bottom up
    for (int depth = int(log2(BLOCK_ELEMENTS)) - 1; depth >= 0; depth--){
        stride = 1 << depth;    // This is equal to stride = 2^depth

        uint writeIndex = (localId + 1) * (stride*2) - 1; // Only odd indexes
        uint readIndex = writeIndex - stride;

        if(writeIndex < BLOCK_ELEMENTS){
            int temp = sharedTotalParticlesPerTask[readIndex];
            sharedTotalParticlesPerTask[readIndex] = sharedTotalParticlesPerTask[writeIndex];
            sharedTotalParticlesPerTask[writeIndex] += temp;
        }

        groupMemoryBarrier(); barrier(); // Ensure shared memory writes are complete
    }

    // Make the prefix sum INCLUSIVE
    sharedTotalParticlesPerTask[localId] += val1;
    sharedTotalParticlesPerTask[localId + WG_SIZE] += val2;
    
    groupMemoryBarrier();  // Ensure shared memory writes are complete
    

    // Write the results back to global memory
    if(secondPass){
        blockSums[groupId * BLOCK_ELEMENTS + localId] = sharedTotalParticlesPerTask[localId];
        blockSums[groupId * BLOCK_ELEMENTS + localId + WG_SIZE] = sharedTotalParticlesPerTask[localId + WG_SIZE];
    }
    else {
        tasks[groupId * BLOCK_ELEMENTS + localId].totalParticles = sharedTotalParticlesPerTask[localId];
        tasks[groupId * BLOCK_ELEMENTS + localId + WG_SIZE].totalParticles = sharedTotalParticlesPerTask[localId + WG_SIZE];
    }

}