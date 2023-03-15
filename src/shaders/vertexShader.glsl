#version 440 core

layout (location = 0) in vec3 particlePosition;   // the position variable has attribute position 0
layout (location = 1) in vec3 velocity; // the velocity variable has attribute position 1

out vec3 vel;

// 3d view stuff
uniform mat4 modelViewProjection;


void main()
{
    gl_Position = modelViewProjection * vec4(particlePosition, 1.0);
    vel = velocity; // pass the velocity to the fragment shader
}