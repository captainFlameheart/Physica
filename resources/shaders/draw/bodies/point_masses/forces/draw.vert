void main()
{
	uint index = gl_VertexID >> 1u;
	uint endpoint_index = gl_VertexID & 1u;;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint position_velocity_base = fixed_data.point_mass_position_velocity_base;
	uint write_acceleration_turn_flags_base = fixed_data.point_mass_write_acceleration_turn_flags_base;

	uint position_velocity_index = position_velocity_base + index;
	uint write_acceleration_turn_flags_index = write_acceleration_turn_flags_base + index;

	uvec4 position_velocity = uvec4_data.data[position_velocity_index];
	uvec4 write_acceleration_turn_flags = uvec4_data.data[write_acceleration_turn_flags_index];

	uvec2 endpoint_position = position_velocity.xy + position_velocity.zw + write_acceleration_turn_flags.xy * endpoint_index;

	vec4 camera_offset = vec4(ivec2(endpoint_position.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
