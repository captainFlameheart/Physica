const uint offset_index_to_endpoint_index[6u] = uint[6u]
(
	0,
	1,
	1,

	1,
	0,
	0
);

const float offset_index_to_orthogonal_direction_sign[6u] = float[6u]
(
	1.0,
	1.0,
	-1.0,

	-1.0,
	-1.0,
	1.0
);

out vec4 color;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_flags_base = fixed_data.rigid_body_circle_contact_constraint_spawner_indices_flags_base;

	uint indices_flags_index = indices_flags_base + index;

	uvec4 indices_flags = uvec4_data.data[indices_flags_index];

	uint circle_body_position_radius_base = fixed_data.rigid_body_circle_body_position_radius_base;

	uint circle_body_position_radius_index_0 = circle_body_position_radius_base + indices_flags.x;
	uint circle_body_position_radius_index_1 = circle_body_position_radius_base + indices_flags.y;

	uvec4 circle_body_position_radius_0 = uvec4_data.data[circle_body_position_radius_index_0];
	uvec4 circle_body_position_radius_1 = uvec4_data.data[circle_body_position_radius_index_1];

	vec3 circle_position_radius_0 = uintBitsToFloat(circle_body_position_radius_0.yzw);
	vec3 circle_position_radius_1 = uintBitsToFloat(circle_body_position_radius_1.yzw);

	uint rigid_body_write_position_flags_base = fixed_data.rigid_body_write_position_flags_base;

	uint rigid_body_write_position_flags_index_0 = rigid_body_write_position_flags_base + circle_body_position_radius_0.x;
	uint rigid_body_write_position_flags_index_1 = rigid_body_write_position_flags_base + circle_body_position_radius_1.x;

	uvec4 rigid_body_position_flags_0 = uvec4_data.data[rigid_body_write_position_flags_index_0];
	uvec4 rigid_body_position_flags_1 = uvec4_data.data[rigid_body_write_position_flags_index_1];

	uvec4 rigid_body_position_flags[2u] = uvec4[2u]
	(
		rigid_body_position_flags_0,
		rigid_body_position_flags_1
	);

	vec2 direction = normalize(vec2(ivec2(rigid_body_position_flags_0.xy - rigid_body_position_flags_1.xy)));
	vec2 orthogonal_direction = vec2(-direction.y, direction.x);

	uint endpoint_index = offset_index_to_endpoint_index[offset_index];
	uvec4 endpoint_rigid_body_position_flags = rigid_body_position_flags[endpoint_index];

	float direction_sign = float(int(1u - 2u * endpoint_index));
	float orthogonal_direction_sign = offset_index_to_orthogonal_direction_sign[offset_index];

	vec4 camera_offset = vec4(ivec2(endpoint_rigid_body_position_flags.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;

	// TODO: Handle large angle precision.
	float endpoint_rigid_body_angle = float(rigid_body_position_flags[endpoint_index]) * angle_unit_in_radians;

	float angle_cos = cos(endpoint_rigid_body_angle);
	float angle_sin = sin(endpoint_rigid_body_angle);

	vec2 circle_positions[2u] = vec2[2u]
	(
		circle_position_radius_0.xy,
		circle_position_radius_1.xy
	);

	vec2 endpoint_circle_position = circle_positions[endpoint_index];
	
	camera_offset.xy += mat2
	(
		angle_cos, angle_sin,
		-angle_sin, angle_cos
	) * endpoint_circle_position;

	camera_offset.xy += (0.0 * direction_sign) * direction + (0.02 * orthogonal_direction_sign) * orthogonal_direction;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
	
	/*if (gl_VertexID == 0u)
	{
		gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	}
	else if (gl_VertexID == 1u)
	{
		gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	}
	else if (gl_VertexID == 2u)
	{
		gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	}*/

	color = mix
	(
		vec4(1.0, 1.0, 0.0, 0.2),
		vec4(1.0, 0.0, 0.0, 0.2),
		float((indices_flags.z >> rigid_body_circle_contact_constraint_spawner_is_dead_shift) & 1u)
	);
}
