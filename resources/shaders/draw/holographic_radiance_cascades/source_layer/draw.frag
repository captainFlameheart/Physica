layout(binding = source_image_texture_unit) uniform sampler2DArray source;

layout (location = 0) out vec4 color;

void main()
{
	uint layer = fixed_data.visible_source_image_layer - 1u;
	color = texelFetch(source, ivec3(ivec2(gl_FragCoord.xy), layer), 0);
}
