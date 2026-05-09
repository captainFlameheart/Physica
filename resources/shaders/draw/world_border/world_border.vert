const uint vertices_per_edge = 6u;
const uint total_vertex_count = 24u;

const int max_coordinate = 1000000000;
const int min_coordinate = (-max_coordinate) - 1;
const uint thickness = uint(10.0 * meter_in_length_units);

const ivec2 vertices[total_vertex_count] = ivec2[total_vertex_count]
(
	ivec2(max_coordinate, max_coordinate),
	ivec2(max_coordinate - thickness, max_coordinate - thickness),
	ivec2(max_coordinate - thickness, min_coordinate + thickness),

	ivec2(max_coordinate - thickness, min_coordinate + thickness),
	ivec2(max_coordinate, min_coordinate),
	ivec2(max_coordinate, max_coordinate),


	ivec2(max_coordinate, max_coordinate),
	ivec2(min_coordinate, max_coordinate),
	ivec2(min_coordinate + thickness, max_coordinate - thickness),

	ivec2(min_coordinate + thickness, max_coordinate - thickness),
	ivec2(max_coordinate, max_coordinate - thickness),
	ivec2(max_coordinate, max_coordinate),


	ivec2(min_coordinate + thickness, max_coordinate - thickness),
	ivec2(min_coordinate, max_coordinate),
	ivec2(min_coordinate, min_coordinate),

	ivec2(min_coordinate, min_coordinate),
	ivec2(min_coordinate + thickness, min_coordinate + thickness),
	ivec2(min_coordinate + thickness, max_coordinate - thickness),


	ivec2(max_coordinate - thickness, min_coordinate + thickness),
	ivec2(min_coordinate + thickness, min_coordinate + thickness),
	ivec2(min_coordinate, min_coordinate),

	ivec2(min_coordinate, min_coordinate),
	ivec2(max_coordinate, min_coordinate),
	ivec2(max_coordinate - thickness, min_coordinate + thickness)
);

void main()
{
	ivec2 position = vertices[gl_VertexID];
	
	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	vec4 camera_offset = vec4(position - ivec2(camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;	// TODO: Merge into camera_offset_to_clip_coordinates once everything is in length units.
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
