const float end_points[2u] = float[2u](0.75f, 1.5f);

void main()
{
	float start_point = end_points[0u];
	float end_point = end_points[1u];
	
	float start_cycle = float(int(start_point));
	start_point -= start_cycle;
	end_point -= start_cycle;

	float first_line_end = min(end_point, 1.0);
	
	float does_wrap = float(int(end_point));
	float second_line_start = end_point * (1.0 - does_wrap);
	end_point = end_point - does_wrap;

	float points[4u] = float[4u](start_point, first_line_end, second_line_start, end_point);

	float point = points[gl_VertexID];
	float normalized_point = point * 2.0 - 1.0;
	gl_Position = vec4(normalized_point, 0.0, 0.0, 1.0);
}
