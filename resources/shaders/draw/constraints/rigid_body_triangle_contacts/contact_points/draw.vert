float gl_CullDistance[1u];

const float radius = 0.1 * meter_in_length_units;

const vec2 vertices[6u] =
{
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0),

	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),
};

out vec2 offset;

void main()
{
	uint index = gl_VertexID / 12u;
	uint vertex_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_turns_base = fixed_data.rigid_body_triangle_contact_indices_turns_base;

	uint indices_turns_index = indices_turns_base + index;

	uvec4 indices_turns = uvec4_data.data[indices_turns_index];

	uint triangle_body_material_vertex_0_base = fixed_data.rigid_body_triangle_body_material_vertex_0_base;
	uint triangle_vertex_1_2_base = fixed_data.rigid_body_triangle_vertex_1_2_base;

	uint triangle_body_material_vertex_0_index[2u] =
	{
		triangle_body_material_vertex_0_base + indices_turns.x,
		triangle_body_material_vertex_0_base + indices_turns.y,
	};

	uint triangle_vertex_1_2_index[2u] =
	{
		triangle_vertex_1_2_base + indices[0u],
		triangle_vertex_1_2_base + indices[1u],
	};

	uvec4 triangle_body_material_vertex_0[2u] =
	{
		uvec4_data.data[triangle_body_material_vertex_0_index[0u]],
		uvec4_data.data[triangle_body_material_vertex_0_index[1u]],
	};

	vec4 triangle_vertex_1_2[2u] =
	{
		vec4_data.data[triangle_vertex_1_2_index[0u]],
		vec4_data.data[triangle_vertex_1_2_index[1u]],
	};

	uint body_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint body_position_flags_index[2u] =
	{
		body_position_flags_base + triangle_body_material_vertex_0[0u].x,
		body_position_flags_base + triangle_body_material_vertex_0[1u].x,
	};

	ivec4 body_position_flags[2u] =
	{
		ivec4(uvec4_data.data[body_position_flags_index[0u]]),
		ivec4(uvec4_data.data[body_position_flags_index[1u]]),
	};

	vec2 body_position_difference = vec2(ivec2(rigid_body_position_flags[1u].xy) - ivec2(rigid_body_position_flags[0u].xy));

	// TODO: Handle large angle precision.
	float angles[2u] =
	{
		float(int(rigid_body_position_flags[0u].z)) * angle_unit_in_radians,
		float(int(rigid_body_position_flags[1u].z)) * angle_unit_in_radians,
	};
	vec2 rights[2u] =
	{
		vec2(cos(angles[0u]), sin(angles[0u])),
		vec2(cos(angles[1u]), sin(angles[1u])),
	};
	mat2 body_local_to_offset_transforms[2u] =
	{
		mat2(rights[0u], vec2(-rights[0u].y, rights[0u].x)),
		mat2(rights[1u], vec2(-rights[1u].y, rights[1u].x)),
	};
	vec2 offsets[2u] =
	{
		body_local_to_offset_transforms[0u] * (meter_in_length_units * vec2(0.0)),
		body_local_to_offset_transforms[1u] * (meter_in_length_units * vec2(0.0)),
	};

	float body_0_radius_fraction = 0.5;
	vec2 body_0_contact_point_offset = body_0_radius_fraction * (body_position_difference + (offsets[1u] - offsets[0u]));

	vec4 camera_offset = vec4(ivec2(rigid_body_position_flags[0u].xy - camera_position.xy), int(-camera_position.z), 1.0f);
	offset = vertices[vertex_index];
	camera_offset.xy += offset * radius;
	camera_offset.xy += body_0_contact_point_offset;
	camera_offset.xyz *= length_unit_in_meters;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;

	gl_CullDistance[0u] = 1.0;
}
