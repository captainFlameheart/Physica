const uint vertices_per_timestamp = 6u;

const vec2 bar_vertices[] =
{
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, 0.0),

	vec2(-1.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
};

flat out vec4 timestamp_color;

void main()
{
	uint timestamp_index = gl_VertexID / vertices_per_timestamp;
	uint local_index = gl_VertexID - timestamp_index * vertices_per_timestamp;

	vec2 bar_vertex = bar_vertices[local_index];
	
	const float bar_half_width = 0.01;
	const float bar_height = 0.5;

	gl_Position = vec4(bar_vertex.x * bar_half_width, bar_vertex.y * bar_height - 1.0, 0.0, 1.0);

	timestamp_color = vec4(1.0);
}
