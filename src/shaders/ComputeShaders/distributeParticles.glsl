#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

struct Task {
    int root;
    int totalParticles;
};

layout(std430, binding=9) buffer tasksBuffer
{
    Task tasks[];
};

layout(std430, binding=10) buffer particleOffsetsBuffer
{
    ivec2 particleOffsets[];    // .x = storing offset, .y = nodeId
};

layout(std430, binding=12) buffer taskParticlesIndexesBuffer
{
    int taskParticlesIndexes[];
};


uniform int totalParticles;
uniform int totalTasks;



int getTotalParticles(int taskId) {
    return (taskId >= 0) ? tasks[taskId].totalParticles: 0;
}

void main(){
    uint particleId = gl_GlobalInvocationID.x;

    if (particleId >= totalParticles) return;

    const int nodeId = particleOffsets[particleId].y;
    const int taskId = nodeId % totalTasks; 
    
    const int prevTaskTotalParticles = getTotalParticles(taskId-1);
    const int offset = particleOffsets[particleId].x;

    
    taskParticlesIndexes[prevTaskTotalParticles+offset] = int(particleId);

}