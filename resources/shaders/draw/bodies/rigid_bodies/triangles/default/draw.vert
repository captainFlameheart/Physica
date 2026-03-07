void main()
{
	uint index = gl_VertexID / 3u;
	
	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint body_base = fixed_data.rigid_body_triangle_body_base;
	uint vertex_base = fixed_data.rigid_body_triangle_vertex_base;

	uint body_index = body_base + index;
	uint vertex_index = vertex_base + gl_VertexID;

	uint body = uint_data.data[body_index];
	vec2 vertex = vec2_data.data[vertex_index];

	uint body_write_position_turn_base = fixed_data.rigid_body_write_position_turn_base;

	uint body_write_body_position_turn_index = body_write_position_turn_base + body;

	uvec4 body_position_turn = uvec4_data.data[body_write_body_position_turn_index];

	// TODO: Handle large angle precision.
	float angle = float(int(body_position_turn.z)) * angle_unit_in_radians;
	float angle_cos = cos(angle);
	float angle_sin = sin(angle);
	vertex = mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * vertex;

	vec4 camera_offset = vec4(ivec2(body_position_turn.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += vertex;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
