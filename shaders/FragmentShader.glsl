#version 330
// Input

in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;
in float noise_height;

uniform vec3 Object_Color;
uniform int isTeren;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec3 out_texture;
// layout(location = 0) out vec4 out_position;
 


void main()
{
    if (isTeren == 1) {
        vec3 highColor = vec3(0.2, 0.9, 0.3); 
        vec3 lowColor = vec3(0.1, 0.45, 0.25); 
        vec3 terrainColor = mix(lowColor, highColor, noise_height);
        out_color = vec4(terrainColor, 1.0);
    } else {
        out_color = vec4(Object_Color, 1.0);
    }

}