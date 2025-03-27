#version 460 core

layout (location = 0) in vec3 position;

uniform float x;

void main()
{
	gl_Position = vec4(position.x + x, position.y, position.z, 1.0);
}
