#version 460 core

layout(location = 0) in vec2 fragPos;

layout(location = 0) out vec4 colour;

uniform int u_maxIterations;
uniform float u_brightness;
uniform float u_zoom;
uniform vec2 u_center;

// Squares an complex number
vec2 squareImaginary(vec2 num)
{
	return vec2(num.x * num.x - num.y * num.y, 2.0*num.x*num.y);
}

void main()
{
	// Scale the fragment position based on zoom
	vec2 scaledPos = fragPos * u_zoom + u_center;
	vec2 z = scaledPos;
	colour = vec4(0.0, 0.0, 0.0, 1.0); // Set the colour to black if it didn't escape
	for (int i = 0; i < u_maxIterations; i++)
	{
		z = squareImaginary(z) + scaledPos;
		if (length(z) > 2.0)
		{
			colour = vec4(float(i) / u_brightness, float(i) / u_brightness * 2, float(i) / u_brightness * 4, 1.0);
			//colour = vec4(0.0, 4 * float(i) / u_brightness, 2 * float(i) / u_brightness, 1.0);
			return;
		}
	}
}