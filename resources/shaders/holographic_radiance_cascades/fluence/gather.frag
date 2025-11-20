/* Expected to be concatenated from CPU:



*/

layout(shared, binding = FLUENCE_GATHERING_BINDING) uniform Fluence_Gathering_Data
{
} fluence_gathering_data;

uniform sampler2DArray rays;
uniform sampler2D higher_cascade_fluence;

layout (location = 0) out vec4 fluence;

void main()
{
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	
	int direction_id = output_texel_position.x

	int ray_id_in_column = ray_casting_data.skipped_rays_below_column + output_texel_position.y;
	int probe_y = ray_id_in_column / ray_casting_data.rays_per_probe;
	int direction_id = ray_id_in_column - probe_y * ray_casting_data.rays_per_probe;

	texelFetch();
}
