#define RADIUS (GRAB_RADIUS + LIGHT_DISTANCE)

const vec2 offsets[6u] = vec2[6u]
(
	vec2(RADIUS, RADIUS), 
	vec2(-RADIUS, RADIUS), 
	vec2(-RADIUS, -RADIUS), 

	vec2(-RADIUS, -RADIUS), 
	vec2(RADIUS, -RADIUS), 
	vec2(RADIUS, RADIUS)
);

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

noperspective out vec2 offset;

void main()
{
	uint contact_index = gl_VertexID.x / 6u;
	ivec2 source_position = gravity_sources.positions[contact_index];

	uint local_index = gl_VertexID.x % 6u;
	offset = offsets[local_index];

	vec2 camera_relative_xy = vec2(source_position - camera.xy) + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
