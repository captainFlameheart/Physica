layout(shared, binding = CONTACT_BINDING) restrict readonly
buffer Contacts
{
	uvec2 contacts[MAX_CONTACT_COUNT];
} contacts;

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
	ivec4 bounding_box = bounding_boxes.boxes[leaf];
	ivec2 world_position = bounding_box.xy + (bounding_box.zw - bounding_box.xy) / 2;
	
	ivec2 camera_relative_xy = world_position - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
