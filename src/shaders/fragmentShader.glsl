#version 440 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 velColor;
void main()
{
    // Make GL_POINTS circular
    vec2 pos = gl_PointCoord.xy-0.5;
    if(dot(pos,pos) > 0.25){
        discard;
    }

    // Set the color of the particle
    FragColor = vec4(velColor, 1.0f);

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.7){
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
    else{
        BrightColor = vec4(0.0, 0.0, 0.0, 0.0);
    }

}