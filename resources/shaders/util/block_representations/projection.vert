#version 460 core

#define PROJECTION_BINDING 0

layout(shared, binding = PROJECTION_BINDING) uniform Projection
{
	vec2 scaling;
} projection;

void main()
{
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
