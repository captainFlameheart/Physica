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

#define CULL_DEAD 0
#define CULL_ALIVE 0

#if CULL_DEAD || CULL_ALIVE
	out float gl_CullDistance[1u];
#endif

out vec4 color;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;

	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint indices_base = fixed_data.bounding_box_contact_detector_indices_base;

	uint indices_index = indices_base + index;

	uvec2 indices = uvec2_data.data[indices_index];

	uint bounding_box_base = fixed_data.inner_bounding_box_base;
	uint bounding_box_parent_children_height_base = fixed_data.inner_bounding_box_parent_children_height_base;

	uint bounding_box_index_0 = bounding_box_base + indices.x;
	uint bounding_box_index_1 = bounding_box_base + indices.y;

	uint bounding_box_parent_children_height_index_0 = bounding_box_parent_children_height_base + indices.x;
	uint bounding_box_parent_children_height_index_1 = bounding_box_parent_children_height_base + indices.y;

	uvec4 bounding_boxes[2u] = uvec4[2u]
	(
		uvec4_data.data[bounding_box_index_0],
		uvec4_data.data[bounding_box_index_1]
	);

	uvec4 bounding_box_parent_children_heights[2u] = uvec4[2u]
	(
		uvec4_data.data[bounding_box_parent_children_height_index_0],
		uvec4_data.data[bounding_box_parent_children_height_index_1]
	);

	vec2 camera_offset_0_min = vec2(ivec2(bounding_boxes[0u].xy - camera_position.xy));
	vec2 camera_offset_0_max = vec2(ivec2(bounding_boxes[0u].zw - camera_position.xy));

	vec2 camera_offset_1_min = vec2(ivec2(bounding_boxes[1u].xy - camera_position.xy));
	vec2 camera_offset_1_max = vec2(ivec2(bounding_boxes[1u].zw - camera_position.xy));

	vec2 camera_offsets[2u] = vec2[2u]
	(
		0.5 * (camera_offset_0_min + camera_offset_0_max),
		0.5 * (camera_offset_1_min + camera_offset_1_max)
	);
	vec2 direction = normalize(camera_offsets[0u] - camera_offsets[1u]);
	vec2 orthogonal_direction = vec2(-direction.y, direction.x);

	uint endpoint_index = offset_index_to_endpoint_index[offset_index];

	float direction_sign = float(int(1u - 2u * endpoint_index));
	float orthogonal_direction_sign = offset_index_to_orthogonal_direction_sign[offset_index];

	vec4 camera_offset = vec4(camera_offsets[endpoint_index], int(-camera_position.z), 1.0);
	camera_offset *= length_unit_in_meters;

	camera_offset.xy += (0.0 * direction_sign) * direction + (0.02 * orthogonal_direction_sign) * orthogonal_direction;
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;

	uint height = bounding_box_parent_children_heights[endpoint_index].w;
	color = mix
	(
		leaf_bounding_box_color,
		inner_bounding_box_height_colors[height % inner_bounding_box_height_colors.length()],
		float(int(indices[endpoint_index]) >= 0)
	);

	#if CULL_DEAD
	#endif
	#if CULL_ALIVE
		gl_CullDistance[0u] = -1.0;
	#endif
}
