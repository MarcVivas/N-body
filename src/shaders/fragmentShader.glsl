#version 440 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float worldSize;

in vec4 particleVelocity;


vec4 getParticleColor(){
    // Define a maximum velocity value
    float maxVelocity = worldSize/11.f;

    // Compute the magnitude of the particle's velocity
    float velocityMagnitude = length(particleVelocity);

    // Compute a normalized velocity value between 0 and 1
    float normalizedVelocity = clamp(velocityMagnitude / maxVelocity, 0.0, 1.0);

    // Define two colors for the gradient (e.g. blue and yellow)
    vec3 colorLow = vec3(0.0, 0.0, 1.0); // blue (slow)
    vec3 colorHigh = vec3(1.0, 1.0, 0.0); // yellow (fast)

    // Interpolate between the two colors based on the normalized velocity value
    float smoothNormalizedVelocity = smoothstep(0.0, 1.0, normalizedVelocity);

    return vec4(mix(colorLow, colorHigh, smoothNormalizedVelocity), 1); // pass the velocity to the fragment shader
}


vec4 getParticleBrightColor(){
    // Extract the bright particles
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 0.7){
        // Glow the particle
        return vec4(FragColor.rgb, 1.0);
    }

    // Do not glow this particle
    return vec4(0.0, 0.0, 0.0, 0.0);
}

void main()
{
    // Make GL_POINTS circular
    vec2 pos = gl_PointCoord.xy-0.5;
    if(dot(pos,pos) > 0.25){
        discard;
    }

    // Set the color of the particle
    FragColor = getParticleColor();

    // Render in the second framebuffer the bright particles
    BrightColor = getParticleBrightColor();
}