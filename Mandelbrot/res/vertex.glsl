#version 460 core

layout(location = 0) in vec4 vertPos;

layout(location = 0) out vec2 fragPos;

uniform mat2 u_MVP;
uniform vec2 u_center;


void main()
{
	fragPos = u_MVP * vertPos.xy;
	gl_Position = vertPos;
}