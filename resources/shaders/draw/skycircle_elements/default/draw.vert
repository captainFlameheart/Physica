const float angle_to_sky_circle_coordinate_factor = (0.5 / pi) * (angle_unit_in_radians);

//uniform ivec2 end_points;

flat out vec4 color;
noperspective out float offset;

void main()
{
	uint element_index = gl_VertexID >> 2u;
	uint local_id = gl_VertexID & 3u;
	ivec4 end_points_velocity_flags = ivec4(uvec4_data.data[fixed_data.default_skycircle_element_end_points_velocity_flags_base + element_index]);

	// TODO: Handle large angles in fixed point

	const int base_angle = int(fixed_data.camera_angle);
	float start_point = float(end_points_velocity_flags[0u] - base_angle) * angle_to_sky_circle_coordinate_factor;
	float end_point = float(end_points_velocity_flags[1u] - base_angle) * angle_to_sky_circle_coordinate_factor;

	float point_distance = end_point - start_point;
	
	float start_cycle = float(floor(start_point));
	start_point -= start_cycle;
	end_point -= start_cycle;

	float first_line_end = min(end_point, 1.0);
	
	float does_wrap = float(int(end_point));
	float second_line_start = end_point * (1.0 - does_wrap);
	end_point = end_point - does_wrap;

	float points[4u] = float[4u](start_point, first_line_end, second_line_start, end_point);

	float point = points[local_id];
	float normalized_point = point * 2.0 - 1.0;
	gl_Position = vec4(normalized_point, 0.0, 0.0, 1.0);

	float first_line_fraction = (first_line_end - start_point) / point_distance;
	float offsets[4u] = float[4u](0.0, first_line_fraction, first_line_fraction, 1.0);
	offset = offsets[local_id];

	color = vec4_data.data[fixed_data.default_skycircle_element_radiance_base + element_index];
}
