void main()
{
	uint index = gl_VertexID >> 1u;
	uint endpoint_index = gl_VertexID & 1u;;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint write_position_velocity_base = fixed_data.point_mass_write_position_velocity_base;

	uint write_position_velocity_index = write_position_velocity_base + index;

	uvec4 position_velocity = uvec4_data.data[write_position_velocity_index];
	
	uvec2 endpoint_position = position_velocity.xy + position_velocity.zw * endpoint_index;

	vec4 camera_offset = vec4(ivec2(endpoint_position.xy - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
