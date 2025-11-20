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
	int g;
	int f;
	int lower_cascade_rays_per_probe;
	int lower_cascade_skipped_rays_below_column;
} ray_casting_data;

uniform sampler2DArray shorter_rays;

layout (location = 0) out vec4 radiance;
layout (location = 1) out vec4 transmittance;

void main()
{
	// TODO: CLAMP

	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
	int ray_id_in_column = ray_casting_data.skipped_rays_below_column + output_texel_position.y;
	int probe_y = ray_id_in_column / ray_casting_data.rays_per_probe;
	int direction_id = ray_id_in_column - probe_y * ray_casting_data.rays_per_probe;

	// Some temporaries
	int a = (output_texel_position.x << 1) | 1;
	int b = ray_casting_data.lower_cascade_rays_per_probe * probe_y - ray_casting_data.lower_cascade_skipped_rays_below_column;
	int d = direction_id >> 1;
	int h = b + d;

	// IMPORTANT TODO: Use 16 bit floats for radiance and transmittance to reduce from 8 to 4 texel fetches.

	// Lower near
	radiance = texelFetch(shorter_rays, ivec3(a, h, 0), 0);
	transmittance = texelFetch(shorter_rays, ivec3(a, h, 1), 0);
	
	// And some more temporaries needed from here
	int e = (direction_id + 1) >> 1;
	int i = b + e;
	int c = a + 1;

	// Lower far
	int lower_far_sample_y = i + d * ray_casting_data.g - ray_casting_data.f;
	radiance += transmittance * texelFetch(shorter_rays, ivec3(c, lower_far_sample_y, 0), 0);
	transmittance *= texelFetch(shorter_rays, ivec3(c, lower_far_sample_y, 1), 0);

	// Upper near
	radiance += texelFetch(shorter_rays, ivec3(a, i, 0), 0);
	vec4 upper_transmittance = texelFetch(shorter_rays, ivec3(a, i, 1), 0);
	
	// Upper far
	int upper_far_sample_y = h + e * ray_casting_data.g - ray_casting_data.f;
	vec4 radiance += upper_near_transmittance * texelFetch(shorter_rays, ivec3(c, upper_far_sample_y, 0), 0);
	upper_transmittance *= texelFetch(shorter_rays, ivec3(c, upper_far_sample_y, 1), 0)
	
	transmittance += upper_transmittance;

	radiance *= 0.5;
	transmittance *= 0.5;
}
