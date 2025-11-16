/* Expected to be concatenated from CPU:



*/

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

layout(shared, binding = RAY_CASTING_BINDING) uniform Ray_Casting_Data
{
	int skipped_rays_below_column;
	int rays_per_probe;
} ray_casting_data;

uniform sampler2DArray shorter_rays;

layout (location = 0) out vec4 radiance;
layout (location = 1) out vec4 transmittance;

void main()
{
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int ray_id_in_column = ray_casting_data.skipped_rays_below_column + output_texel_position.y;
	int probe_y = ray_id_in_column / ray_casting_data.rays_per_probe;
	int direction_id = ray_id_in_column - probe_y * ray_casting_data.rays_per_probe;

	// IMPORTANT TODO: Use 16 bit floats for radiance and transmittance to reduce from 8 to 4 texel fetches.
	vec4 radiance = texelFetch(shorter_rays, ivec3(0, 0, 0), 0);
	vec4 transmittance = texelFetch(shorter_rays, ivec3(0, 0, 1), 0);
}
