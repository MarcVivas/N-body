#version 440 core

layout(std430, binding = 0) buffer positionsBuffer {
    vec4 positions[];
};

layout(std430, binding = 1) buffer velocitiesBuffer {
    vec4 velocities[];
};

out vec4 particleVelocity;
out float particleSize;

uniform mat4 modelViewProjection;
uniform vec3 cameraPos;
uniform float worldSize;
uniform bool pointSize; // if true, scales the size dynamically
uniform int totalParticles;

float getParticleSize(){
    // Base size is reduced to avoid overly large points.
    const float baseSize = 1.0;  
    // Adjust size based on the distance from the camera.
    const float distanceFactor = length(cameraPos - positions[gl_VertexID].xyz);
    const float size = baseSize / distanceFactor;
    
    if(totalParticles < 1000) return max(size * worldSize, 3);

    if(pointSize)
        return max(size * worldSize, 1);
    return baseSize;
}

void main()
{
    gl_PointSize = getParticleSize();
    particleSize = gl_PointSize;
    gl_Position = modelViewProjection * vec4(positions[gl_VertexID].xyz, 1.0);
    particleVelocity = velocities[gl_VertexID];
}