#define RADIUS 0.1 * METER

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
	float target_velocity;
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
	Contact_Surface contact_surfaces[MAX_CONTACT_COUNT];
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
	uint contact_index = gl_VertexID / 4u;
	Contact_Surface contact_surface = contact_surfaces.contact_surfaces[contact_index];
	
	uint body_0 = contact_surface.bodies[0u];
	uint body_1 = contact_surface.bodies[1u];

	vec2 body_0_to_1 = METER_INVERSE * vec2(positions.p[body_1].xy - positions.p[body_0].xy);

	vec2 offset_0_0 = contact_surface.contact_point_positions[0u].offsets[0u];
	vec2 offset_0_1 = contact_surface.contact_point_positions[0u].offsets[1u];
	vec2 offset_1_0 = contact_surface.contact_point_positions[1u].offsets[0u];
	vec2 offset_1_1 = contact_surface.contact_point_positions[1u].offsets[1u];

	offset_0_0 = vec2(offset_0_0.y, -offset_0_0.x);
	offset_0_1 = body_0_to_1 + vec2(offset_0_1.y, -offset_0_1.x);
	offset_1_0 = vec2(offset_1_0.y, -offset_1_0.x);
	offset_1_1 = body_0_to_1 + vec2(offset_1_1.y, -offset_1_1.x);

	vec2 offset = METER * 0.25 * (offset_0_0 + offset_0_1 + offset_1_0 + offset_1_1);

	vec2 direction = contact_surface.tangent;
	if (gl_VertexID % 4u > 1u)
	{
		direction = vec2(-direction.y, direction.x);
	}
	offset += (float(gl_VertexID % 2u) * RADIUS) * direction;

	vec2 camera_relative_xy = vec2(positions.p[body_0].xy - camera.xy) + offset;

	if (contact_surface.contact_point_tangents[0u].mass == 0.0)
	{
		camera_relative_xy.x = 1E20;
	}

	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
