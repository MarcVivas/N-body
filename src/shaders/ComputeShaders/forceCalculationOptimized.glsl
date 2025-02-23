#version 430 core

#define BLOCK_SIZE 320

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;

uniform float squaredSoftening;
uniform int numParticles;

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=3) buffer massesBuffer
{
    vec4 masses[];
};

layout(std430, binding=4) buffer forcesBuffer
{
    vec4 forces[];
};

shared vec3 blockPositions[BLOCK_SIZE];
shared float blockMasses[BLOCK_SIZE];

void main() {
    const uint index = gl_GlobalInvocationID.x % numParticles;
    const bool isValidIndex = gl_GlobalInvocationID.x < numParticles;

    const float G = 1.0;
    vec3 totalForce = vec3(0.0);
    vec3 particlePosition = positions[index].xyz;
    const uint blockIndex = gl_GlobalInvocationID.x % BLOCK_SIZE;

    for(uint b = 0; b < numParticles; b+=BLOCK_SIZE){

        // Store particles into shared memory
        const uint particleIndex = blockIndex + b;
        if(particleIndex < numParticles){
            blockPositions[blockIndex] = positions[particleIndex].xyz;
            blockMasses[blockIndex] = masses[particleIndex].x;
        }
        else{
            blockPositions[blockIndex] = vec3(0);
            blockMasses[blockIndex] = 0.0;
        }

        // Ensure shared memory writes are visible to work group
        memoryBarrierShared();

        // Ensure all threads in work group have executed statements above
        barrier();

        if (isValidIndex){
            for(uint j = 0; j < BLOCK_SIZE; j++){
                if(index != j){
                    const vec3 vector_i_j = blockPositions[j] - particlePosition;
                    const float distance_i_j = pow(dot(vector_i_j, vector_i_j) + squaredSoftening, 1.5);
                    totalForce += ((G * blockMasses[j]) / distance_i_j) * vector_i_j;
                }

            }
        }

        // Ensure all threads in work group have executed statements above
        barrier();
    }

    if(isValidIndex){
        // Write to global memory the result
        forces[index] = vec4(totalForce, 0.0);
    }

}
