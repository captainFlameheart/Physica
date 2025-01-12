#version 460 core

layout (location = 0) in vec3 p;

uniform float x;

void main()
{
	gl_Position = vec4(p.x + x, p.y, p.z, 1.0);
}
