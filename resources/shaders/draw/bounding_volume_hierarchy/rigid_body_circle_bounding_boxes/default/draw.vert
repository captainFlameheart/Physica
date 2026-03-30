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

out vec2 UV;
flat out vec2 size;

void main()
{
	uint index = gl_VertexID / 6u;
	uint offset_index = gl_VertexID - index * 6u;
	UV = UVs[offset_index];
	
	uvec4 camera_position = fixed_data.camera_position;
	mat4 camera_offset_to_clip_coordinates = fixed_data.camera_offset_to_clip_coordinates;

	uint bounding_box_base = fixed_data.leaf_bounding_box_bases[rigid_body_circle_bounding_box_type];

	uint bounding_box_index = bounding_box_base + index;

	uvec4 bounding_box = uvec4_data.data[bounding_box_index];

	size = vec2(ivec2(bounding_box.zw - bounding_box.xy));

	uvec2 vertex_indices = bounding_box_vertex_indices[offset_index];
	uvec2 vertex = uvec2(bounding_box[vertex_indices.x], bounding_box[vertex_indices.y]);

	vec4 camera_offset = vec4(ivec2(vertex - camera_position.xy), int(-camera_position.z), 1.0f);
	camera_offset.xyz *= length_unit_in_meters;	// TODO: Merge into camera_offset_to_clip_coordinates once everything is in length units.
	gl_Position = camera_offset_to_clip_coordinates * camera_offset;
}
