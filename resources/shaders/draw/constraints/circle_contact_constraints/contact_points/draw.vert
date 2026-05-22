const float radius = 0.1 * meter_in_length_units;

const vec2 vertices[6u] =
{
	vec2(radius, radius),
	vec2(-radius, radius),
	vec2(-radius, -radius),

	vec2(-radius, -radius),
	vec2(radius, -radius),
	vec2(radius, radius),
};

void main()
{
	uint index = gl_VertexID / 6u;
	uint vertex_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_turns_base = fixed_data.rigid_body_circle_contact_constraint_indices_turns_base;

	uint indices_turns_index = indices_turns_base + index;

	uvec4 indices_turns = uvec4_data.data[indices_turns_index];

	uint rigid_body_circle_body_position_radius_base = fixed_data.rigid_body_circle_body_position_radius_base;
	
	uint rigid_body_circle_body_position_radius_index[2u] =
	{
		rigid_body_circle_body_position_radius_base + indices_turns.x,
		rigid_body_circle_body_position_radius_base + indices_turns.y,
	};

	uvec4 rigid_body_circle_body_position_radius[2u] =
	{
		uvec4_data.data[rigid_body_circle_body_position_radius_index[0u]],
		uvec4_data.data[rigid_body_circle_body_position_radius_index[1u]],
	};
	
	vec3 rigid_body_circle_position_radius[2u] =
	{
		vec3(rigid_body_circle_body_position_radius[0u]),
		vec3(rigid_body_circle_body_position_radius[1u]),
	};

	uint rigid_body_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint rigid_body_position_flags_index[2u] =
	{
		rigid_body_position_flags_base + rigid_body_circle_body_position_radius[0u].x,
		rigid_body_position_flags_base + rigid_body_circle_body_position_radius[1u].x,
	};

	uvec4 rigid_body_position_flags[2u] =
	{
		uvec4_data.data[rigid_body_position_flags_index[0u]],
		uvec4_data.data[rigid_body_position_flags_index[1u]],
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
		body_local_to_offset_transforms[0u] * (meter_in_length_units * rigid_body_circle_position_radius[0u].xy),
		body_local_to_offset_transforms[1u] * (meter_in_length_units * rigid_body_circle_position_radius[1u].xy),
	};

	float body_0_radius_fraction = rigid_body_circle_position_radius[0u].z / (rigid_body_circle_position_radius[0u].z + rigid_body_circle_position_radius[1u].z);
	vec2 body_0_contact_point_offset = body_0_radius_fraction * (body_position_difference + (offsets[1u] - offsets[0u]));

	vec4 camera_offset = vec4(ivec2(rigid_body_position_flags[0u].xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += body_0_contact_point_offset;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
