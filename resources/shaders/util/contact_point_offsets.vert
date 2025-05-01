layout(shared, binding = CONTACT_BINDING) restrict readonly
buffer Contacts
{
	uvec2 contacts[MAX_CONTACT_COUNT];
} contacts;

layout(shared, binding = POSITION_BINDING) restrict readonly
buffer Positions
{
	ivec4 p[MAX_RIGID_BODY_COUNT];
} positions;

struct Contact_Point_Position
{
	vec2 offsets[2];
};

struct Contact_Point_Tangent
{
	float mass;
	float impulse;
};

struct Contact_Point_Normal
{
	float mass;
	float impulse;
};

// TODO: See if data can be split up to take advantage of coalescing for a half-warp.
struct Contact_Surface
{
	uint bodies[2];
	Contact_Point_Position contact_point_positions[2];
	
	vec2 tangent;
	Contact_Point_Tangent contact_point_tangents[2];

	Contact_Point_Normal contact_point_normals[2];
};

layout(shared, binding = CONTACT_SURFACE_BINDING) restrict readonly
buffer Contact_Surfaces
{
	Contact_Surface c[MAX_CONTACT_COUNT];
	//Contact_Surface contact_surfaces[MAX_CONTACT_COUNT];
} contact_surfaces;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	/*
	uint contact_index = gl_VertexID / 8u;
	Contact_Surface contact_surface = contact_surfaces.contact_surfaces[contact_index];

	uint endpoint_index = gl_VertexID % 8u;
	
	uint local_body_index = (endpoint_index / 2u) % 2u;
	uint body_index = contact_surface.bodies[local_body_index];

	uint contact_point_index = endpoint_index / 4u;
	Contact_Point_Position contact_point_position = contact_surface.contact_point_positions[contact_point_index];
	uint at_offset_tip = endpoint_index % 2u;
	vec2 offset = float(at_offset_tip) * contact_point_position.offsets[body_index];

	vec2 camera_relative_xy = vec2(positions.p[body_index].xy - camera.xy) + offset;
	*/
	vec2 camera_relative_xy = vec2(0.0, 0.0);
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
