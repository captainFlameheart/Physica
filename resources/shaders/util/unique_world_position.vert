layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	vec2 right;
	int z;
} camera;

layout(location = 0) in ivec2 world_position;

void main()
{
	ivec2 camera_relative_xy = world_position - camera.xy;
	mat2 view_rotation = mat2
	(
		camera.right.x, -camera.right.y
		camera.right.y, camera.right.x
	);
	gl_Position = vec4(view_rotation * camera_relative_xy * PROJECTION_SCALE, 0.0, camera.z);
	/*ivec2 view_position = ivec2
	(
		camera_relative_xy.x * camera.right.x + camera_relative_xy.y * camera.right.y, 
		camera_relative_xy.y * camera.right.x - camera_relative_xy.x * camera.right.y
	);*/
	/*if (gl_VertexID == 0)
	{
		gl_Position = vec4(ivec2(-1000, -1000) * PROJECTION_SCALE, 0.0, camera.z);
	}
	else
	{
		gl_Position = vec4(view_position * PROJECTION_SCALE, 0.0, camera.z);
	}*/
}
