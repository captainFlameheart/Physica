// TODO: It MIGHT be faster with more triangles to reduce the number of fragments
const vec2 offsets[6u] = vec2[6u]
(
	vec2(RADIUS, RADIUS), 
	vec2(-RADIUS, RADIUS), 
	vec2(-RADIUS, -RADIUS), 

	vec2(-RADIUS, -RADIUS), 
	vec2(RADIUS, -RADIUS), 
	vec2(RADIUS, RADIUS)
);

layout(shared, binding = FLUID_POSITION_BINDING) restrict readonly
buffer Fluid_Position
{
	ivec2 p[MAX_FLUID_PARTICLE_COUNT];
} fluid_position;

layout(shared, binding = FLUID_VELOCITY_BINDING) restrict readonly
buffer Fluid_Velocity
{
	ivec2 v[MAX_FLUID_PARTICLE_COUNT];
} fluid_velocity;

/*
struct Fluid_Draw_Type
{
	float red;
	float green;
	float blue;
	float alpha;
	float radius;
};

layout(shared, binding = CAMERA_BINDING) uniform Fluid_Draw_Types
{
	Fluid_Draw_Type types[FLUID_TYPE_COUNT];
} fluid_draw_types;
*/
layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

noperspective out vec2 offset;
noperspective out vec4 base_color;

void main()
{
	uint particle_index = gl_VertexID / 6u;
	ivec2 position = fluid_position.p[particle_index];
	ivec2 velocity = fluid_velocity.v[particle_index];

	uint offset_index = gl_VertexID % 6u;
	offset = offsets[offset_index];

	base_color = mix(vec4(1.0, 0.0, 0.0, 1.3), vec4(1.0, 1.0, 0.5, 1.2), min(1.0, 0.000003 * length(velocity)));

	vec2 camera_relative_xy = vec2(position - camera.xy) + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
