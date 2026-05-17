// TODO: Add friction data?
struct Fluid_Triangle_Contact
{
	uint triangle;
	float mass;
	float impulse;
	uint particle;
	uint body;
	float target_velocity;
	vec2 offset;
	vec2 direction;
	vec2 impulse_range;
};

layout(shared, binding = FLUID_TRIANGLE_CONTACT_BINDING) restrict readonly
buffer Fluid_Triangle_Contacts
{
	Fluid_Triangle_Contact contacts[MAX_FLUID_TRIANGLE_CONTACT_COUNT];
} fluid_triangle_contacts;

layout(shared, binding = POSITION_BINDING) restrict readonly
buffer Positions
{
	ivec4 p[MAX_RIGID_BODY_COUNT];
} positions;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	if (fluid_triangle_contacts.contacts[gl_VertexID].mass == 0.0)
	{
		gl_Position = vec4(0.0, 0.0, 0.0, -1.0);
		return;
	}
	uint body = fluid_triangle_contacts.contacts[gl_VertexID].body;
	ivec4 body_position = positions.p[body];
	vec2 offset = fluid_triangle_contacts.contacts[gl_VertexID].offset;
	offset = METER * vec2(offset.y, -offset.x);
	vec2 camera_relative_xy = vec2(body_position.xy - camera.xy) + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
