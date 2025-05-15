layout(shared, binding = FLUID_POSITION_BINDING) restrict readonly
buffer Fluid_Position
{
	ivec2 p[MAX_FLUID_PARTICLE_COUNT];
} fluid_position;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	ivec2 position = fluid_position.p[gl_VertexID];

	vec2 camera_relative_xy = vec2(position - camera.xy);
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
