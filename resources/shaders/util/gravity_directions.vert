#define X_STEP (STEP * PROJECTION_SCALE.x)
#define Y_STEP (STEP * PROJECTION_SCALE.y)
#define WIDTH (1u + 2u * uint(1.0 / X_STEP))
#define HEIGHT (1u + 2u * uint(1.0 / Y_STEP))

layout(shared, binding = GRAVITY_SOURCES_BINDING) uniform Gravity_Sources
{
	uint count;
	ivec2 positions[MAX_GRAVITY_SOURCE_COUNT];
	float strengths[MAX_GRAVITY_SOURCE_COUNT];
} gravity_sources;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uint grid_point_index = gl_VertexID / 2u;

	uint x_index = grid_point_index % WIDTH;
	float x_sign = 2.0 * float(x_index % 2u) - 1.0;
	float x_abs = float((x_index + 1u) / 2u) * X_STEP;

	uint y_index = grid_point_index / WIDTH;
	float y_sign = (2.0 * float(y_index % 2u) - 1.0);
	float y_abs = float((y_index + 1u) / 2u) * Y_STEP;

	float x = x_sign * x_abs;
	float y = y_sign * y_abs;

	vec2 screen_position = vec2(x, y);
	float angle = float(camera.angle) * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);
	mat2 camera_local_to_offset = transpose(camera.view_rotation);
	ivec2 world_position = camera.xy + ivec2(camera_local_to_offset * (camera.z * screen_position / PROJECTION_SCALE));
	vec2 force = vec2(0.0, 0.0);
	for (uint i = 0u; i < gravity_sources.count; ++i)
	{
		ivec2 source_position = gravity_sources.positions[i];
		float strength = gravity_sources.strengths[i];
		vec2 offset = METER_INVERSE * (source_position - world_position);
		force += normalize(offset) * METER * (strength * exp(-dot(offset, offset) / (strength * strength)));
	}
	vec2 force_offset = float(gl_VertexID % 2u) * 0.5 * force;

	vec2 camera_relative_xy = (world_position - camera.xy) + force_offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
