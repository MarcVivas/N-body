#version 430 core

layout( local_size_x = 64, local_size_y =1, local_size_z = 1  ) in;

uniform int numParticles;
uniform float deltaTime;

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=1) buffer velocitiesBuffer
{
    vec4 velocities[];
};

layout(std430, binding=2) buffer accelerationsBuffer
{
    vec4 accelerations[];
};

layout(std430, binding=4) buffer massesBuffer
{
    vec4 masses[];
};

layout(std430, binding=4) buffer forcesBuffer
{
    vec4 forces[];
};


void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index < numParticles) {

        vec3 pos_t = positions[index].xyz; // x(t)
        vec3 vel_t = velocities[index].xyz; // v(t)
        vec3 force_t = forces[index].xyz;   // F(t)

        // Calculate acceleration a(t) from force F(t)
        vec3 acc_t = force_t; 

        // --- Semi-Implicit Euler ---
        // 1. Update velocity first using a(t)
        vec3 vel_next = vel_t + acc_t * deltaTime; // v(t+dt) = v(t) + a(t)*dt

        // 2. Update position using the *new* velocity
        vec3 pos_next = pos_t + vel_next * deltaTime; // x(t+dt) = x(t) + v(t+dt)*dt

        // --- Update Buffers ---
        positions[index].xyz = pos_next;
        velocities[index].xyz = vel_next;
    }

}
