#version 440 core

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=1) buffer velocitiesBuffer
{
    vec4 velocities[];
};

out vec4 particleVelocity;

// 3d view stuff
uniform mat4 modelViewProjection;

uniform vec3 cameraPos;

uniform float worldSize;

uniform bool pointSize;


float getParticleSize(){
    // set the particle size based on the distance from the camera to the particle
    float particleSize = 1.25 / length(cameraPos - positions[gl_VertexID].xyz);

    // set the point size based on the particle size
    if(pointSize){
        return particleSize * worldSize;
    }
    return 1;
}


void main()
{
    // Set the point size
    gl_PointSize = getParticleSize();

    // Set the position of the particle
    gl_Position = modelViewProjection * vec4(positions[gl_VertexID].xyz, 1.f);

    // Pass the velocity to the fragment shader
    particleVelocity = velocities[gl_VertexID];
}