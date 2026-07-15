#version 460 core

layout(location = 0) in vec3 fragPos;

layout(location = 0) out vec4 colour;

uniform int u_maxIterations;
uniform float u_brightness;
uniform float u_zoom;
uniform vec2 u_center;
uniform vec2 u_startingPos;

// Squares an complex number
vec2 squareImaginary(vec2 num)
{
	return vec2(num.x * num.x - num.y * num.y, 2.0*num.x*num.y);
}

void main()
{
    // Scale the fragment position based on zoom
    vec2 scaledPos = fragPos.xy * u_zoom + u_center;
    vec2 z = scaledPos.xy;
    colour = vec4(0.0, 0.0, 0.0, 1.0); // Set the colour to black if it didn't escape
    for (int i = 0; i < u_maxIterations; i++)
    {
        z = squareImaginary(z) + u_startingPos;
        if (length(z) > 10.0)
        {
            colour = vec4(float(i) / u_brightness, float(i) / u_brightness * 2, float(i) / u_brightness * 4 + 0.2, 1.0);
            return;
        }
    }
}