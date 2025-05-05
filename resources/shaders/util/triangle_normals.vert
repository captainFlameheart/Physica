#define LENGTH 0.5 * METER

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
	uint triangle_index = gl_VertexID / 6u;
	uvec4 triangle = triangles.triangles[triangle_index];
	
	uint normal_index = (gl_VertexID % 6u) / 2u;
	vec2 edge_start = vertices.vertices[triangle[normal_index]];
	vec2 edge_end = vertices.vertices[triangle[(normal_index + 1u) % 3u]];
	vec2 tangent = normalize(edge_start - edge_end);
	vec2 normal = vec2(-tangent.y, tangent.x);
	float at_tip = float(gl_VertexID % 2u);
	vec2 local_position = 0.5 * (edge_start + edge_end) + at_tip * LENGTH * normal;
	
	ivec4 body_position = positions.p[triangle.w];

	// TODO: Make sure of no precision loss for large angles
	float angle = body_position.z * RADIAN_INVERSE;
	float right_x = cos(angle);
	float right_y = sin(angle);

	// TODO: Check if matrix construction and multiplication is faster!
	vec2 offset = vec2
	(
		local_position.x * right_x - local_position.y * right_y, 
		local_position.x * right_y + local_position.y * right_x
	);

	ivec2 camera_relative_body_xy = body_position.xy - camera.xy;
	vec2 camera_relative_vertex_xy = camera_relative_body_xy + offset;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_vertex_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
}
