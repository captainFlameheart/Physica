uniform uint hovered_triangle;

layout(shared, binding = TRIANGLE_BINDING) restrict readonly
buffer Triangles
{
	uvec4 triangles[MAX_TRIANGLE_COUNT];	// Triangle: (vertex_0, vertex_1, vertex_2, body)
} triangles;

layout(shared, binding = VERTEX_BINDING) restrict readonly
buffer Vertices
{
	vec2 vertices[MAX_VERTEX_COUNT];
} vertices;

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
	uvec4 triangle = triangles.triangles[hovered_triangle];
	
	uint local_vertex_index = (gl_VertexID + 1u) / 2u % 3u;
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
	);
}
