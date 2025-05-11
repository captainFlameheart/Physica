layout(shared, binding = CURSOR_CONSTRAINED_POINT_BINDING) uniform Cursor_Constrained_Point
{
	uint body;
	vec2 local_point;
} cursor_constrained_point;

layout(shared, binding = POSITION_BINDING) restrict readonly
buffer Positions
{
	ivec4 p[MAX_RIGID_BODY_COUNT];
} positions;

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
	if (gl_VertexID.x == 0u)
	{
		uint body = cursor_constrained_point.body;
		vec2 local_point = cursor_constrained_point.local_point;

		vec4 body_position = positions.p[body];
		
		float angle = body_position.z * RADIAN_INVERSE;
		float right_x = cos(angle);
		float right_y = sin(angle);

		// TODO: See if it's faster to construct and multiply with a matrix
		vec2 offset = local_point.x * vec2(right_x, right_y) + local_point.y * vec2(-right_y, right_x);

		vec2 camera_relative_xy = vec2(body_position.xy - camera.xy) + offset;
		gl_Position = vec4
		(
			camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
			0.0, 
			camera.z
		);
	}
	else
	{
		vec2 camera_relative_xy = cursor_position.position - camera.xy;
		gl_Position = vec4
		(
			camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
			0.0, 
			camera.z
		);
	}
}
