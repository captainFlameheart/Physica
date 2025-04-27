const uint x_indices[] = uint[](2u, 0u, 0u, 0u, 0u, 2u, 2u, 2u);
const uint y_indices[] = uint[](3u, 3u, 3u, 1u, 1u, 1u, 1u, 3u);

uniform ivec4 bounding_box;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uint x_index = x_indices[gl_VertexID];
	uint y_index = y_indices[gl_VertexID];
	ivec2 vertex = ivec2(bounding_box[x_index], bounding_box[y_index]);

	ivec2 camera_relative_vertex_xy = vertex - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
