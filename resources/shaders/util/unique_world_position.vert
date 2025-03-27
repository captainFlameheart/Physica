#version 460 core

#define PROJECTION_BINDING 0
#define CAMERA_BINDING 1

layout(shared, binding = PROJECTION_BINDING) uniform Projection
{
	vec2 scaling;
} projection;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	vec4 transform;
} camera;

layout(location = 0) in vec2 world_position;

void main()
{
	gl_Position = vec4(world_position.x, world_position.y, 0.0, 1.0);
}
