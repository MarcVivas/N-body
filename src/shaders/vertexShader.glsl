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


void main()
{
    gl_Position = modelViewProjection * vec4(positions[gl_VertexID].xyz, 1.0);
    vel = velocities[gl_VertexID].xyz; // pass the velocity to the fragment shader
}