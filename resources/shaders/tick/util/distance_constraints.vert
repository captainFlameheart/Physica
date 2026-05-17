struct Distance_Constraint
{
	uvec2 bodies;
	vec2 local_points[2u];
	vec2 offsets[2u];
	vec2 direction;
	float max_distance;
	float target_velocity;
	float mass;
	float impulse;
};

layout(shared, binding = DISTANCE_CONSTRAINT_BINDING) restrict readonly
buffer Distance_Constraints
{
	uint count;
	Distance_Constraint distance_constraints[MAX_DISTANCE_CONSTRAINT_COUNT];
} distance_constraints;

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
	uint distance_constraint_index = gl_VertexID / 2u;
	uint local_index = gl_VertexID % 2u;

	uint body = distance_constraints.distance_constraints[distance_constraint_index].bodies[local_index];
	vec2 local_point = distance_constraints.distance_constraints[distance_constraint_index].local_points[local_index];

	vec4 body_position = positions.p[body];
		
	float angle = body_position.z * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);

	// TODO: See if it's faster to construct and multiply with a matrix
	vec2 offset = local_point.x * vec2(right_x, right_y) + local_point.y * vec2(-right_y, right_x);

	vec2 camera_relative_xy = vec2(body_position.xy - camera.xy) + offset * METER;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
