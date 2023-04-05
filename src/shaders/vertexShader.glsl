#version 440 core

layout(std430, binding=0) buffer positionsBuffer
{
    vec4 positions[];
};

layout(std430, binding=1) buffer velocitiesBuffer
{
    vec4 velocities[];
};

out vec3 velColor;

// 3d view stuff
uniform mat4 modelViewProjection;

uniform vec3 cameraPos;

uniform float worldSize;

uniform bool pointSize;

void main()
{

    // set the particle size based on the distance from the camera to the particle
    float particleSize = 1.0 / length(cameraPos - positions[gl_VertexID].xyz);

    // set the point size based on the particle size
    if(pointSize){
        gl_PointSize = particleSize * worldSize;
    }
    else{
        gl_PointSize = 1;
    }


    // Set the position of the particle
    gl_Position = modelViewProjection * vec4(positions[gl_VertexID].xyz, 1.f);

    // Define a maximum velocity value
    float maxVelocity = worldSize/11.f;

    // Compute the magnitude of the particle's velocity
    float velocityMagnitude = length(velocities[gl_VertexID]);

    // Compute a normalized velocity value between 0 and 1
    float normalizedVelocity = clamp(velocityMagnitude / maxVelocity, 0.0, 1.0);

    // Define two colors for the gradient (e.g. blue and yellow)
    vec3 colorLow = vec3(0.0, 0.0, 1.0); // blue (slow)
    vec3 colorHigh = vec3(1.0, 1.0, 0.0); // yellow (fast)

    // Interpolate between the two colors based on the normalized velocity value
    float smoothNormalizedVelocity = smoothstep(0.0, 1.0, normalizedVelocity);
    velColor = mix(colorLow, colorHigh, smoothNormalizedVelocity); // pass the velocity to the fragment shader
}