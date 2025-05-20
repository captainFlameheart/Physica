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

layout(shared, binding = FLUID_POSITION_BINDING) restrict readonly
buffer Fluid_Position
{
	ivec2 p[MAX_FLUID_PARTICLE_COUNT];
} fluid_position;

layout(shared, binding = TRIANGLE_BINDING) restrict readonly
buffer Triangles
{
	uvec4 triangles[MAX_TRIANGLE_COUNT];	// Triangle: (vertex_0, vertex_1, vertex_2, body)
} triangles;

layout(shared, binding = POSITION_BINDING) restrict readonly
buffer Positions
{
	ivec4 p[MAX_RIGID_BODY_COUNT];
} positions;

layout(shared, binding = VERTEX_BINDING) restrict readonly
buffer Vertices
{
	vec2 vertices[MAX_VERTEX_COUNT];
} vertices;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uint contact_index = gl_VertexID / 2u;
	vec2 camera_relative_xy;
	if (gl_VertexID % 2u == 0u)
	{
		uint particle = fluid_triangle_contacts.contacts[contact_index].particle;
		ivec2 position = fluid_position.p[particle];
		camera_relative_xy = vec2(position - camera.xy);
	}
	else
	{
		uint triangle_index = fluid_triangle_contacts.contacts[contact_index].triangle;
		uvec4 triangle = triangles.triangles[triangle_index];
		ivec4 body_position = positions.p[triangle.w];
		float angle = body_position.z * RADIAN_INVERSE;
		float right_x = cos(angle);
		float right_y = sin(angle);
		mat2 local_to_offset = mat2
		(
			right_x, right_y, 
			-right_y, right_x
		);
		vec2 local_vertex_0 = vertices.vertices[triangle[0u]];
		vec2 local_vertex_1 = vertices.vertices[triangle[1u]];
		vec2 local_vertex_2 = vertices.vertices[triangle[2u]];
		vec2 local_center = 0.333 * (local_vertex_0 + local_vertex_1 + local_vertex_2);
		camera_relative_xy = (body_position.xy - camera.xy) + local_to_offset * local_center;
	}
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
