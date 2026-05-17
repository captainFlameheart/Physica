// TODO: Move to CPU
#define RADIUS 0.5 * METER

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
	ivec4 p[MAX_RIGID_BODY_COUNT];
} positions;

void main()
{
	int local_index = int(gl_VertexID % 4u);
	int line_index = local_index >> 1;
	vec2 local_position = vec2
	(
		float((1 - line_index) * ((local_index << 1) - 1)) * RADIUS, 
		float(line_index * (((local_index - 2) << 1) - 1) ) * RADIUS
	);

	uint body_index = gl_VertexID / 4u;
	ivec4 body_position = positions.p[body_index];
	float angle = body_position.z * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);
	vec2 offset = vec2
	(
		local_position.x * right_x - local_position.y * right_y, 
		local_position.x * right_y + local_position.y * right_x
	);
	vec2 camera_relative_xy = vec2(body_position.xy - camera.xy) + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
