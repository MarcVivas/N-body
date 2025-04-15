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

uniform int totalTasks;

void main(){
    uint index = gl_GlobalInvocationID.x;
    if(index >= totalTasks){
        return;
    }
    tasks[index].totalParticles = 0;
}