#version 460 core

layout(location = 0) in vec4 vertPos;

layout(location = 0) out vec3 fragPos;

uniform mat3 u_MVP; // TODO: This is actually the inverse projection matrix
uniform mat3 u_transform;

void main()
{
	fragPos = u_MVP * vertPos.xyz;
	gl_Position = mat4(u_transform) * vertPos;
}