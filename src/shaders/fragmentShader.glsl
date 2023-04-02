#version 440 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 vel;
void main()
{
    // Make GL_POINTS circular
    vec2 pos = gl_PointCoord.xy-0.5;
    if(dot(pos,pos) > 0.25){
        discard;
    }

    FragColor = vec4(vel, 1.0f);
}