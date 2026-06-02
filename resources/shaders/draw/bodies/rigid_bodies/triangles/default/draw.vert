void main()
{
	uint index = gl_VertexID / 3u;
	uint local_index = gl_VertexID - 3u * index;
	
	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint body_material_vertex_0_base = fixed_data.rigid_body_triangle_body_material_vertex_0_base;
	uint vertex_1_2_base = fixed_data.rigid_body_triangle_vertex_1_2_base;

	uint body_material_vertex_0_index = body_material_vertex_0_base + index;
	uint vertex_1_2_index = vertex_1_2_base + index;

	uvec4 body_material_vertex_0 = uvec4_data.data[body_material_vertex_0_index];
	vec4 vertex_1_2 = vec4_data.data[vertex_1_2_index];

	vec2 vertices[3u] =
	{
		uintBitsToFloat(body_material_vertex_0.zw),
		vertex_1_2.xy,
		vertex_1_2.zw,
	};
	vec2 vertex = vertices[local_index];

	uint body_write_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint body_write_body_position_flags_index = body_write_position_flags_base + body_material_vertex_0.x;

	uvec4 body_position_flags = uvec4_data.data[body_write_body_position_flags_index];

	// TODO: Handle large angle precision.
	float angle = float(int(body_position_flags.z)) * angle_unit_in_radians;
	float angle_cos = cos(angle);
	float angle_sin = sin(angle);
	vertex = mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * vertex;

	vec4 camera_offset = vec4(ivec2(body_position_flags.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += vertex;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
