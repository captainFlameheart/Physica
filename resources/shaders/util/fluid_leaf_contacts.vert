struct Fluid_Contact
{
	bool inactive;
	float impulse;
	uvec2 particles;
	vec2 direction;
	float target_velocity;
	float mass;
	vec2 impulse_range;
};

layout(shared, binding = FLUID_CONTACT_BINDING) restrict
buffer Fluid_Contacts
{
	Fluid_Contact contacts[MAX_FLUID_CONTACT_COUNT];
} fluid_contacts;

layout(shared, binding = FLUID_POSITION_BINDING) restrict readonly
buffer Fluid_Position
{
	ivec4 p[MAX_FLUID_PARTICLE_COUNT];//ivec2 p[MAX_FLUID_PARTICLE_COUNT];
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
	uint particle = fluid_contacts.contacts[gl_VertexID / 2u].particles[gl_VertexID % 2u];
	ivec4 position = fluid_position.p[particle];
	ivec2 camera_relative_xy = position.xy - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
