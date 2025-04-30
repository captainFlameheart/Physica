layout(shared, binding = CONTACT_BINDING) restrict readonly
buffer Contacts
{
	uvec2 contacts[MAX_CONTACT_COUNT];
} contacts;

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

layout(shared, binding = BOUNDING_BOX_BINDING) restrict readonly
buffer Bounding_Boxes
{
	ivec4 boxes[MAX_TRIANGLE_COUNT];	// Bounding box: (min_x, min_y, max_x, max_z)
} bounding_boxes;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uvec2 contact = contacts.contacts[gl_VertexID / 2u];
	uint leaf = contact[gl_VertexID % 2u];

	uvec4 triangle = triangles.triangles[leaf];
	ivec4 body_position = positions.p[triangle.w];
	float angle = body_position.z * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);
	mat2 local_to_offset = mat2
	(
		right_x, right_y, 
		-right_y, right_x
	);
	vec2 local_vertex_0 = vertices.vertices[triangle[0]];
	vec2 local_vertex_1 = vertices.vertices[triangle[1]];
	vec2 local_vertex_2 = vertices.vertices[triangle[2]];
	vec2 local_center = 0.333 * (local_vertex_0 + local_vertex_1 + local_vertex_2);
	ivec2 world_position = body_position.xy + ivec2(local_to_offset * local_center);

	/*ivec4 bounding_box = bounding_boxes.boxes[leaf];
	ivec2 world_position = bounding_box.xy + (bounding_box.zw - bounding_box.xy) / 2;*/
	
	ivec2 camera_relative_xy = world_position - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
