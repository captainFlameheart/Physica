const uint vertices_per_timestamp = 6u;

const vec2 vertices[] =
{
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, 0.0),

	vec2(-1.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
};

flat out vec4 deadline_color;

const uint generation_in_length_units_uint = 1000u;
const float generation_in_length_units = float(generation_in_length_units_uint);
const float length_unit_in_generations = 1.0 / generation_in_length_units;
const float millisecond_in_nanoseconds = 1000000.0;
const float nanosecond_in_milliseconds = 1.0 / millisecond_in_nanoseconds;

const float step_wise_bar_half_width = 0.01;
const float smooth_bar_half_width = 0.001;

void main()
{
	const float chart_base = -1.0;
	const float bar_half_width = smooth_bar_half_width;
	const float bar_width = 2.0 * bar_half_width;
	const float deadline_height = 0.5;
	const float deadline_milliseconds = 1000.0 / 60.0;
	const float millisecond_height = deadline_height / deadline_milliseconds;
	const float nanosecond_height = nanosecond_in_milliseconds * millisecond_height;
	const float stroke_half_thickness = 0.01;

	vec2 vertex = vertices[gl_VertexID];
	gl_Position = vec4(vertex.x, chart_base + deadline_height + vertex.y * stroke_half_thickness, 0.0, 1.0);

	deadline_color = vec4(1.0, 0.0, 0.0, 1.0);
}
