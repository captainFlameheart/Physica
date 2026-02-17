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

const float density = 1.0 / (0.02 * pi);
const float pi_times_density = pi * density;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_turns_base = fixed_data.point_mass_distance_constraint_indices_turns_base;

	uint indices_turns_index = indices_turns_base + index;

	uvec4 indices_turns = uvec4_data.data[indices_turns_index];

	uint point_mass_position_velocity_base = fixed_data.point_mass_position_velocity_base;

	uint point_mass_position_velocity_index_0 = point_mass_position_velocity_base + indices_turns.x;
	uint point_mass_position_velocity_index_1 = point_mass_position_velocity_base + indices_turns.y;

	uvec4 point_mass_position_velocities[2u] = uvec4[2u](
		uvec4_data.data[point_mass_position_velocity_index_0],
		uvec4_data.data[point_mass_position_velocity_index_1]
	);

	vec2 direction = normalize(vec2(ivec2(point_mass_position_velocities[0u].xy - point_mass_position_velocities[1u].xy)));
	vec2 orthogonal_direction = vec2(-direction.y, direction.x);

	uint endpoint_index = offset_index_to_endpoint_index[offset_index];
	uvec4 endpoint_position_velocity = point_mass_position_velocities[endpoint_index];

	float direction_sign = float(int(1u - 2u * endpoint_index));
	float orthogonal_direction_sign = offset_index_to_orthogonal_direction_sign[offset_index];

	vec4 camera_offset = vec4(ivec2(endpoint_position_velocity.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	camera_offset.xy += (0.0 * direction_sign) * direction + (0.005 * orthogonal_direction_sign) * orthogonal_direction;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
