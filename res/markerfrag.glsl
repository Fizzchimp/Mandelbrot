#version 460 core

layout(location = 0) in vec3 fragPos;

layout(location = 0) out vec4 colour;

uniform vec2 u_markerCenter;

void main()
{
    if (length(fragPos.xy - u_markerCenter) <= 1)
    {
        colour = vec4(vec3(0.2 + 0.5 * pow(cos(length(fragPos.xy - u_markerCenter)), 2)), 1.0);
    }
    else
    {
        colour = vec4(0.0);
    }
}