#version 440 core

layout( local_size_x = 64, local_size_y =1, local_size_z = 1  ) in;

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


void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index < numParticles) {


        float G = 1.0f;
        vec4 totalForce = vec4(0);
        vec4 particlePosition = positions[index];


        for (uint j = 0;  j < numParticles; ++j){
            const vec4 vector_i_j = positions[j] - particlePosition;
            const float distance_i_j = pow(dot(vector_i_j, vector_i_j) + squaredSoftening, 1.5f);
            totalForce += ((G * masses[j].x) / distance_i_j) * vector_i_j;
        }


        // Write to global memory the result
        forces[index] = totalForce;

    }

}
