layout(shared, binding = CURSOR_POSITION_BINDING) uniform Cursor_Position
{
	ivec2 position;
} cursor_position;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	vec2 camera_relative_vertex_xy = cursor_position.position - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
