#version 440 core

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=1) buffer velocitiesBuffer
{
    vec4 velocities[];
};

out vec3 vel;

// 3d view stuff
uniform mat4 modelViewProjection;

uniform vec3 cameraPos;

uniform float worldSize;

void main()
{

    // set the particle size based on the distance from the camera to the particle
    float particleSize = 1.0 / length(cameraPos - positions[gl_VertexID].xyz);

    // set the point size based on the particle size
    gl_PointSize = particleSize * worldSize;

    gl_Position = modelViewProjection * vec4(positions[gl_VertexID].xyz, 1.f);
    vel = velocities[gl_VertexID].xyz; // pass the velocity to the fragment shader
}