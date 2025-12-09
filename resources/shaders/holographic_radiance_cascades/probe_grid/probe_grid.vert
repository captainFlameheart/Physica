uniform uvec2 probe_grid_size;

out vec4 line_color;

void main()
{
	uint line_id = gl_VertexID >> 1u;
	const uint vertical_line_count = probe_grid_size.x + 1u;
	uint spacing_direction = min(1, line_id / vertical_line_count);
	uint local_line_id = line_id - spacing_direction * vertical_line_count;
	uint is_endpoint = gl_VertexID & 1u;
	uint line_direction = spacing_direction ^ 1u;
	float spacing_direction_float = float(spacing_direction);
	float line_direction_float = float(line_direction);
	vec2 spacing_direction_vector = vec2(line_direction_float, spacing_direction_float);
	vec2 line_direction_vector = vec2(spacing_direction_float, line_direction_float);

	const vec2 normalized_cell_half_size = 1.0 / (probe_grid_size - 1u);
	//const vec2 normalized_cell_size = 2.0 / (probe_grid_size - 1u);
	const float normalized_line_radius = 1.0 + normalized_cell_half_size[line_direction];
	gl_Position = vec4
	(
		((float(local_line_id << 1u) - 1.0) * normalized_cell_half_size[spacing_direction] - 1.0) * spacing_direction_vector 
		+ ((float(is_endpoint << 1u) - 1.0) * normalized_line_radius) * line_direction_vector,
		0.0, 1.0
	);

	const float brightness = 0.01;
	line_color = vec4(brightness, brightness, brightness, 1.0);
}
