const uint x_indices[] = uint[](2u, 0u, 0u, 0u, 0u, 2u, 2u, 2u);
const uint y_indices[] = uint[](3u, 3u, 3u, 1u, 1u, 1u, 1u, 3u);

layout(shared, binding = FLUID_BOUNDING_BOX_BINDING) restrict readonly
buffer Fluid_Bounding_Boxes
{
	ivec4 boxes[MAX_FLUID_PARTICLE_COUNT];	// Bounding box: (min_x, min_y, max_x, max_y)
} fluid_bounding_boxes;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uint box_index = gl_VertexID / 8u;
	ivec4 box = fluid_bounding_boxes.boxes[box_index];
	uint vertex_index = gl_VertexID % 8u;
	uint x_index = x_indices[vertex_index];
	uint y_index = y_indices[vertex_index];
	ivec2 vertex = ivec2(box[x_index], box[y_index]);

	ivec2 camera_relative_vertex_xy = vertex - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
