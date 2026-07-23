#version 460 core

layout(location = 0) in vec4 vertPos;

layout(location = 0) out vec3 fragPos;

uniform mat3 u_projection;
uniform mat3 u_transform;
uniform mat3 u_translation;

void main()
{
	fragPos = u_translation * vertPos.xyz;
	gl_Position = mat4(u_projection * u_transform) * vertPos;
}