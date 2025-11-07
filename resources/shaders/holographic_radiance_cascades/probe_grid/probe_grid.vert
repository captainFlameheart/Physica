uniform uvec2 probe_grid_size;

out vec4 line_color;

void main()
{
	uint line_id = gl_VertexID >> 1u;
	const uint vertical_line_count = probe_grid_size.x + 1u;
	uint local_line_id = line_id % vertical_line_count;
	uint is_endpoint = gl_VertexID & 1u;

	uint spacing_direction = line_id / vertical_line_count;
	uint line_direction = spacing_direction ^ 1u;
	float spacing_direction_float = float(spacing_direction);
	float line_direction_float = float(line_direction);
	vec2 spacing_direction_vector = vec2(line_direction_float, spacing_direction_float);
	vec2 line_direction_vector = vec2(spacing_direction_float, line_direction_float);

	const vec2 normalized_cell_size = 2.0 / probe_grid_size;
	gl_Position = vec4
	(
		vec2(-1.0) + (float(local_line_id) * normalized_cell_size[spacing_direction]) * spacing_direction_vector + (float(is_endpoint) * 2.0) * line_direction_vector,
		0.0, 1.0
	);

	float brightness = 0.01;
	line_color = vec4(brightness, brightness, brightness, 1.0);
}
