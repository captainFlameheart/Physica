uniform uvec2 probe_grid_size;
uniform uint cascade;

const vec4 cascade_colors[] = 
{
	vec4(0x00, 0x00, 0xFF, 0.2),
	vec4(0x00, 0xFF, 0x00, 0.2),
	vec4(0xFF, 0x00, 0x00, 0.2),
	vec4(0xFF, 0x00, 0xFF, 0.2),
};

out vec4 cone_color;

void main()
{
	uint cascade_power_of_two = 1u << cascade;
	uint vertices_per_probe = cascade_power_of_two * 3u;
	uint vertices_per_probe_column = probe_grid_size.y * vertices_per_probe;
	uint probe_column = gl_VertexID / vertices_per_probe_column;
	uint id_in_probe_column = gl_VertexID - probe_column * vertices_per_probe_column;

	uint probe_x = (1u + probe_column) * cascade_power_of_two;
	uint probe_y = id_in_probe_column / vertices_per_probe;

	uint id_in_probe = id_in_probe_column - probe_y * vertices_per_probe;
	uint lower_direction_index = id_in_probe / 3u;
	uint id_in_cone = id_in_probe - lower_direction_index * 3u;

	float cascade_power_of_two_float = float(cascade_power_of_two);
	int lower_direction_y = int(lower_direction_index << 1u) - int(cascade_power_of_two);
	float lower_direction_y_float = float(lower_direction_y);
	float upper_direction_y_float = float(lower_direction_y + 2);

	const float cone_radius = 0.5;
	float cone_radius_factor = cone_radius / cascade_power_of_two_float;
	float at_lower_end = float(id_in_cone & 1u);
	float at_upper_end = float(id_in_cone >> 1u);
	float at_end = at_lower_end + at_upper_end;
	vec2 position = vec2
	(
		float(probe_x) + cone_radius_factor * (at_end * cascade_power_of_two_float), 
		float(probe_y) + cone_radius_factor * (at_lower_end * lower_direction_y_float + at_upper_end * upper_direction_y_float)
	);
	vec2 normalized_probe_distance = 2.0 / vec2(probe_grid_size - 1u);
	vec2 normalized_position = position * normalized_probe_distance - 1.0;

	gl_Position = vec4
	(
		normalized_position,
		0.0, 1.0
	);

	float color_factor = float(lower_direction_index & 1u);
	cone_color = cascade_colors[cascade % cascade_colors.length()];
	cone_color = vec4((1.0 - 2.0 * color_factor) * cone_color.rgb + color_factor, cone_color.a);
}
