#version 430 core

#define BLOCK_SIZE 256

layout( local_size_x = BLOCK_SIZE, local_size_y =1, local_size_z = 1  ) in;




layout(std430, binding=0) buffer inputPosBuffer
{
    vec4 originalPositions[];
};

layout(std430, binding=1) buffer inputVelBuffer
{
    vec4 originalVelocities[];
};

layout(std430, binding=17) buffer outputPosBuffer
{
    vec4 newPositions[];
};

layout(std430, binding=18) buffer outputVelBuffer
{
    vec4 newVelocities[];
};

struct KeyIndex{
    uint mortonCode;
    uint particleIndex;
};

layout(std430, binding=16) buffer mortonBuffer
{
	readonly KeyIndex mortonCodes[];    
};

uniform int numParticles;




void main(){
    const uint globalId = gl_GlobalInvocationID.x;
    if (globalId >= numParticles) return; // Not you
    
    const uint originalParticleId = mortonCodes[globalId].particleIndex;


    // Write the original position and velocity to the new buffers
    newPositions[globalId] = originalPositions[originalParticleId];
    newVelocities[globalId] = originalVelocities[originalParticleId];

    
}
