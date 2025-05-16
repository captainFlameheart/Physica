struct Fluid_Contact
{
	uvec2 particles;
	vec2 direction;
	float max_impulse;
	float target_velocity;
	float mass;
	float impulse;
};

layout(shared, binding = FLUID_CONTACT_BINDING) restrict
buffer Fluid_Contacts
{
	Fluid_Contact contacts[MAX_FLUID_CONTACT_COUNT];
} fluid_contacts;

layout(shared, binding = FLUID_POSITION_BINDING) restrict readonly
buffer Fluid_Position
{
	ivec2 p[MAX_FLUID_PARTICLE_COUNT];
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
	ivec2 position = fluid_position.p[particle];
	ivec2 camera_relative_xy = position - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
