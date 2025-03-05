#version 330
// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
// layout(location = 2) in vec2 v_texture;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int isTeren;
// uniform vec3 Object_Color;

// Output
out vec3 frag_position;
out vec3 frag_normal;
// out vec2 frag_texture;
out vec3 frag_color;
out float noise_height;

float random (in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));


    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
    vec3 adjustedPosition = v_position;
    if (isTeren == 1) { 
        float frequency = 1.5;
        float amplitude = 0.5;
        float height = noise(v_position.xz * frequency) * amplitude;
        adjustedPosition.y += height;
        noise_height = height;
    } else {
        noise_height = 0.0;
    }

    gl_Position = Projection * View * Model * vec4(adjustedPosition, 1.0);
    frag_color = v_color;
	
}