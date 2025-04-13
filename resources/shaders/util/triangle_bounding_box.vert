const uint x_indices[] = uint[](2u, 0u, 0u, 0u, 0u, 2u, 2u, 2u);
const uint y_indices[] = uint[](3u, 3u, 3u, 1u, 1u, 1u, 1u, 3u);

layout(shared, binding = BOUNDING_BOX_BINDING) restrict readonly
buffer Bounding_Boxes
{
	ivec4 boxes[];	// Bounding box: (min_x, min_y, max_x, max_z)
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
	uint vertex_index = gl_VertexID % 8u;
	uint x_index = x_indices[vertex_index];
	uint y_index = y_indices[vertex_index];

	uint box_index = gl_VertexID / 8u;
	ivec4 box = bounding_boxes.boxes[box_index];
	ivec2 vertex = ivec2(box[x_index], box[y_index]);

	ivec2 camera_relative_vertex_xy = vertex - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);

	/*
	uint triangle_index = gl_VertexID / 3u;
	uvec4 triangle = triangles.triangles[triangle_index];
	
	uint local_vertex_index = gl_VertexID % 3u;
	uint global_vertex_index = triangle[local_vertex_index];
	vec2 local_vertex = vertices.vertices[global_vertex_index];
	
	ivec4 body_position = positions.p[triangle.w];

	// TODO: Make sure of no precision loss for large angles
	float angle = body_position.z * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);

	// TODO: Check if matrix construction and multiplication is faster!
	vec2 vertex_offset = vec2
	(
		local_vertex.x * right_x - local_vertex.y * right_y, 
		local_vertex.x * right_y + local_vertex.y * right_x
	);

	ivec2 camera_relative_body_xy = body_position.xy - camera.xy;
	vec2 camera_relative_vertex_xy = camera_relative_body_xy + vertex_offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);*/
}
