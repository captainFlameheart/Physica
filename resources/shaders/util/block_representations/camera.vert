#version 460 core

#define CAMERA_BINDING 1

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	vec4 transform;
} camera;

void main()
{
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
