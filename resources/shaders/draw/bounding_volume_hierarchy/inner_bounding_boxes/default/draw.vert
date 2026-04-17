float gl_CullDistance[1u];

const vec2 UVs[6u] = vec2[6u]
(
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(0.0, 0.0),

	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0)
);

const uvec2 bounding_box_vertex_indices[6u] = uvec2[6u]
(
	uvec2(2u, 3u),
	uvec2(0u, 3u),
	uvec2(0u, 1u),

	uvec2(0u, 1u),
	uvec2(2u, 1u),
	uvec2(2u, 3u)
);

const vec4 height_colors[] = vec4[]
(
	vec4(1.0, 0.0, 0.0, 0.2),
	vec4(0.0, 1.0, 0.0, 0.2),
	vec4(0.0, 0.0, 1.0, 0.2),
	vec4(1.0, 1.0, 0.0, 0.2),
	vec4(1.0, 0.0, 1.0, 0.2),
	vec4(0.0, 1.0, 1.0, 0.2),
	vec4(1.0, 1.0, 1.0, 0.2)
);

out vec4 color;
out vec2 UV;
flat out vec2 size;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;
	UV = UVs[offset_index];
	
	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint inner_bounding_box_parent_children_height_base = fixed_data.inner_bounding_box_parent_children_height_base;
	uint inner_bounding_box_base = fixed_data.inner_bounding_box_base;

	uint parent_children_height_index = inner_bounding_box_parent_children_height_base + index;
	uint bounding_box_index = inner_bounding_box_base + index;

	uvec4 parent_children_height = uvec4_data.data[parent_children_height_index];
	uvec4 bounding_box = uvec4_data.data[bounding_box_index];

	gl_CullDistance[0u] = 0.0;//-float(parent_children_height.w != 1u);

	color = height_colors[parent_children_height.w % height_colors.length()];

	size = vec2(ivec2(bounding_box.zw - bounding_box.xy));

	uvec2 vertex_indices = bounding_box_vertex_indices[offset_index];
	uvec2 vertex = uvec2(bounding_box[vertex_indices.x], bounding_box[vertex_indices.y]);

	vec4 camera_offset = vec4(ivec2(vertex - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;	// TODO: Merge into camera_offset_to_clip_coordinates once everything is in length units.
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
