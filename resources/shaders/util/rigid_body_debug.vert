layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

layout(shared, binding = POSITION_BINDING) restrict readonly
buffer Positions
{
	ivec3 p[];
} positions;

void main()
{
	int body_index = gl_VertexID / 3;
	ivec3 body_position = positions.p[body_index];
	int local_vertex_index = gl_VertexID % 3;
	float angle_offset = local_vertex_index * 2.094395; // 2 * PI / 3
	float full_angle = body_position.z * RADIAN_INVERSE + angle_offset;
	const float radius = 1.0 * METER;
	vec2 vertex_offset = radius * vec2(cos(full_angle), sin(full_angle));
	ivec2 camera_relative_body_xy = body_position.xy - camera.xy;
	vec2 camera_relative_vertex_xy = camera_relative_body_xy + vertex_offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
