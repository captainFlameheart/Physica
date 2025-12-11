/* Expected to be concatenated from the CPU:

#define SHOWCASE_CASCADE ?
#define SHOWCASE_SINGLE_RAY ?
#define SHOWCASE_MERGE_TO_CONE ?

#define MODE ?;

*/

uniform uvec2 probe_grid_size;
uniform uint cascade;
#if MODE == SHOWCASE_SINGLE_RAY
	uniform uvec2 showcased_ray_texel_position;
#endif
#if MODE == SHOWCASE_MERGE_TO_CONE
	uniform uvec2 merged_to_cone_texel_position;
#endif

#if MODE == SHOWCASE_CASCADE
	const vec4 cascade_colors[] = 
	{
		vec4(0xFF, 0x00, 0xFF, 1.0),
		vec4(0xFF, 0x00, 0x00, 1.0),
		vec4(0x00, 0xFF, 0x00, 1.0),
		vec4(0x00, 0x00, 0xFF, 1.0),
	};
#endif

uvec2 convert_ray_logical_to_texel_position
(
	in uint rays_per_probe, in uint skipped_rays_below_column, 
	in uint probe_column, in uint probe_row, in uint direction_id
)
{
	return uvec2(probe_column, probe_row * rays_per_probe + direction_id - skipped_rays_below_column);
}

vec4 pick_color_if_merging_to
(
	in uvec2 merged_to_texel_position,
	in uint query_rays_per_probe, in uint query_skipped_rays_below_column,
	in uint query_probe_column, in uint query_probe_row, in uint query_direction_id
)
{
	ivec2 output_texel_position = ivec2(merged_to_texel_position);
	ivec2 query_texel_position = ivec2(convert_ray_logical_to_texel_position(
		query_rays_per_probe, query_skipped_rays_below_column,
		query_probe_column, query_probe_row, query_direction_id
	));

	int cascade_merged_to = int(cascade);
	int upper_cascade = cascade_merged_to + 1;
	
	int edge_width = int(probe_grid_size.x) - 1;
	int edge_height = int(probe_grid_size.y) - 1;
	
	int edge_width_decremented = edge_width - 1;

	int fluence_gathering_data_direction_mask = (1 << cascade_merged_to) - 1;
	int fluence_gathering_data_cascade = cascade_merged_to;
	int fluence_gathering_data_cascade_power_of_two = 1 << cascade_merged_to;
	int fluence_gathering_data_upper_cascade_probe_column_texel_x_mask = (-1) << upper_cascade;
	int fluence_gathering_data_max_fluence_probe_column_texel_x = (edge_width_decremented >> upper_cascade) << upper_cascade;
	int fluence_gathering_data_upper_cascade = upper_cascade;
	int fluence_gathering_data_max_fluence_probe_y = edge_height;

	int fluence_gathering_data_max_ray_probe_column = (edge_width - 1) / fluence_gathering_data_cascade_power_of_two - 1;
	int fluence_gathering_data_rays_per_probe = fluence_gathering_data_cascade_power_of_two + 1;
	int fluence_gathering_data_skipped_rays_below_column = (fluence_gathering_data_rays_per_probe + 1) >> 1;
	int fluence_gathering_data_max_ray_probe_row = int(probe_grid_size.y) * fluence_gathering_data_rays_per_probe - (fluence_gathering_data_skipped_rays_below_column << 1) - 1;

	// From here we replicate the fluence gather logic.

	int direction_id = output_texel_position.x & fluence_gathering_data_direction_mask;
	int lower_direction_id = direction_id << 1;
	int probe_column = output_texel_position.x >> fluence_gathering_data_cascade;
	int clamped_near_ray_sample_x = min(probe_column, fluence_gathering_data_max_ray_probe_column);
	float near_ray_is_inside = float(probe_column <= fluence_gathering_data_max_ray_probe_column);

	int lower_near_ray_sample_y = 
		output_texel_position.y * fluence_gathering_data_rays_per_probe - fluence_gathering_data_skipped_rays_below_column 
		+ direction_id;

	float cascade_power_of_two_float = float(fluence_gathering_data_cascade_power_of_two); // TODO: Put in uniform block

	// Lower near ray
	int clamped_lower_near_ray_sample_y = clamp(lower_near_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float lower_near_ray_is_inside = near_ray_is_inside * float(0 <= lower_near_ray_sample_y) * float(lower_near_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);
	
	int interpolating = probe_column & 1;

	int lower_y_offset = lower_direction_id - fluence_gathering_data_cascade_power_of_two;	// TODO: This expression is used by lower angle

	int far_ray_sample_x = probe_column + interpolating;
	int clamped_far_ray_sample_x = min(far_ray_sample_x, fluence_gathering_data_max_ray_probe_column);
	float far_ray_is_inside = float(far_ray_sample_x <= fluence_gathering_data_max_ray_probe_column);
	int lower_far_ray_sample_y = lower_near_ray_sample_y + interpolating * lower_y_offset * fluence_gathering_data_rays_per_probe;

	// Lower far ray
	int clamped_lower_far_ray_sample_y = clamp(lower_far_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float lower_far_ray_is_inside = far_ray_is_inside * float(0 <= lower_far_ray_sample_y) * float(lower_far_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	int not_interpolating = interpolating ^ 1;
	float not_interpolating_float = float(not_interpolating);

	float interpolating_float = float(interpolating);

	int upper_near_ray_sample_y = lower_near_ray_sample_y + 1;

	int upper_direction_id = lower_direction_id + 2;
	int upper_y_offset = upper_direction_id - fluence_gathering_data_cascade_power_of_two; // TODO: This expression is used by upper angle

	// Upper near ray
	int clamped_upper_near_ray_sample_y = clamp(upper_near_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float upper_near_ray_is_inside = near_ray_is_inside * float(0 <= upper_near_ray_sample_y) * float(upper_near_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	int upper_far_ray_sample_y = upper_near_ray_sample_y + interpolating * upper_y_offset * fluence_gathering_data_rays_per_probe;

	// Upper far ray
	int clamped_upper_far_ray_sample_y = clamp(upper_far_ray_sample_y, 0, fluence_gathering_data_max_ray_probe_row);
	float upper_far_ray_is_inside = far_ray_is_inside * float(0 <= upper_far_ray_sample_y) * float(upper_far_ray_sample_y <= fluence_gathering_data_max_ray_probe_row);

	// End of gather replication

	bool query_is_lower_near_ray = query_texel_position.x == clamped_near_ray_sample_x && query_texel_position.y == clamped_lower_near_ray_sample_y;
	bool query_is_lower_far_ray = query_texel_position.x == clamped_far_ray_sample_x && query_texel_position.y == clamped_lower_far_ray_sample_y;
	
	bool query_is_upper_near_ray = query_texel_position.x == clamped_near_ray_sample_x && query_texel_position.y == clamped_upper_near_ray_sample_y;
	bool query_is_upper_far_ray = query_texel_position.x == clamped_far_ray_sample_x && query_texel_position.y == clamped_upper_far_ray_sample_y;

	const float active_alpha = 1.0;

	if (query_is_lower_near_ray)
	{
		return vec4(1.0, 0.0, 0.0, active_alpha);
	}
	if (query_is_lower_far_ray)
	{
		return vec4(0.0, 0.0, 1.0, active_alpha);
	}
	if (query_is_upper_near_ray)
	{
		return vec4(1.0, 1.0, 0.0, active_alpha);
	}
	if (query_is_upper_far_ray)
	{
		return vec4(0.0, 1.0, 1.0, active_alpha);
	}

	const float brightness = 0.2 + float(query_direction_id & 1u) * 0.8;
	return vec4(brightness, brightness, brightness, 0.2);
}

out vec4 line_color;

void main()
{
	uint cascade_power_of_two = 1u << cascade;
	uint lines_per_probe = cascade_power_of_two + 1u;
	uint vertices_per_probe = lines_per_probe << 1u;
	uint vertices_per_probe_column = probe_grid_size.y * vertices_per_probe;
	uint probe_column = gl_VertexID / vertices_per_probe_column;
	uint id_in_probe_column = gl_VertexID - probe_column * vertices_per_probe_column;

	uint probe_x = (1u + probe_column) * cascade_power_of_two;
	uint probe_y = id_in_probe_column / vertices_per_probe;

	uint id_in_probe = id_in_probe_column - probe_y * vertices_per_probe;
	uint direction_index = id_in_probe >> 1u;
	
	#define SHORTEN_DIRECTION 1
	#if SHORTEN_DIRECTION == 1
		vec2 direction = vec2(float(cascade_power_of_two), float(int(direction_index << 1u) - int(cascade_power_of_two)));
		const float removed_length = 0.2;
		float direction_length = length(direction);
		vec2 normalized_direction = direction / direction_length;
		vec2 position = vec2(probe_x, probe_y) + float(gl_VertexID & 1u) * (normalized_direction * (direction_length - removed_length));
		vec2 normalized_probe_distance = 2.0 / vec2(probe_grid_size - 1u);
		vec2 normalized_position = position * normalized_probe_distance - 1.0;

		gl_Position = vec4
		(
			normalized_position,
			0.0, 1.0
		);
	#else
		ivec2 direction = ivec2(cascade_power_of_two, int(direction_index << 1u) - int(cascade_power_of_two));
		ivec2 position = ivec2(probe_x, probe_y) + int(gl_VertexID & 1u) * direction;

		gl_Position = vec4
		(
			float(position.x << 1u) / float(probe_grid_size.x - 1u) - 1.0, 
			float(position.y << 1u) / float(probe_grid_size.y - 1u) - 1.0, 
			0.0, 1.0
		);
	#endif

	#if MODE == SHOWCASE_CASCADE
		float color_factor = float(probe_y & 1u);
		line_color = cascade_colors[cascade % cascade_colors.length()];
		line_color = vec4((1.0 - 2.0 * color_factor) * line_color.rgb + color_factor, 1.0);
	#elif MODE == SHOWCASE_SINGLE_RAY
		uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
		uvec2 ray_texel_position = convert_ray_logical_to_texel_position
		(
			lines_per_probe, skipped_rays_below_column,
			probe_column, probe_y, direction_index
		);
		const float brightness = 0.2 + float(direction_index & 1u) * 0.8;
		line_color = mix(
			vec4(brightness, brightness, brightness, 0.2),
			vec4(0.0, 1.0, 0.0, 1.0),
			float(ray_texel_position == showcased_ray_texel_position)
		);
	#elif MODE == SHOWCASE_MERGE_TO_CONE
		uint skipped_rays_below_column = (lines_per_probe + 1u) >> 1u;
		line_color = pick_color_if_merging_to
		(
			merged_to_cone_texel_position, 
			lines_per_probe, skipped_rays_below_column, probe_column, probe_y, direction_index
		);
	#endif
}
