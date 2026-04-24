const vec2 UVs[6u] = vec2[6u]
(
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0),

	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0)
);

const vec2 offsets[6u] = vec2[6u]
(
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, -1.0),

	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0)
);

out vec2 UV;
flat out uint icon_base;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;
	UV = UVs[offset_index];
	vec2 offset = offsets[offset_index];

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	vec2 normalized_device_coordinates = fixed_data.cursor_normalized_device_coordinates;
	vec2 normalized_device_coordinate_half_size = fixed_data.cursor_normalized_device_coordinate_half_size;
	uint cursor_icon_index = fixed_data.cursor_icon_index;
	icon_base = cursor_icon_index * cursor_icon_length;
	
	vec4 icon_offset_scale = fixed_data.cursor_icon_offset_scales[cursor_icon_index];
	offset += icon_offset_scale.xy;
	offset *= fixed_data.cursor_scale;

	gl_Position = vec4(normalized_device_coordinates + offset * normalized_device_coordinate_half_size, 0.0, 1.0);
}
