/* Expected to be concatenated from CPU:
	
#define EAST_DIRECTION ?
#define NORTH_DIRECTION ?
#define WEST_DIRECTION ?
#define SOUTH_DIRECTION ?

#define DIRECTION ?

const int cascade = ?;
const int angular_step_count = ?;

*/

#if DIRECTION == EAST_DIRECTION || DIRECTION == WEST_DIRECTION
	
	#define X(v) ((v).x)
	#define Y(v) ((v).y)

	#define IVEC2(X, Y) ivec2(X, Y)
	#define IVEC3(X, Y, Z) ivec3(X, Y, Z)
	#define VEC2(X, Y) vec2(X, Y)

#elif DIRECTION == NORTH_DIRECTION || DIRECTION == SOUTH_DIRECTION
	
	#define X(v) ((v).y)
	#define Y(v) ((v).x)

	#define IVEC2(X, Y) ivec2(Y, X)
	#define IVEC3(X, Y, Z) ivec3(Y, X, Z)
	#define VEC2(X, Y) vec2(Y, X)

#endif

const int cascade_power_of_two = 1 << cascade;
const int direction_mask = cascade_power_of_two - 1;
const float cascade_power_of_two_inverse = 1.0 / float(cascade_power_of_two);
const float double_cascade_power_of_two_inverse = cascade_power_of_two_inverse * 2.0;

const float angular_step_fraction = 1.0 / float(angular_step_count);

const float pi = 3.14159265358979323846;
const float half_pi = pi * 0.5;
const float one_and_a_half_pi = pi * 1.5;
const float angle_to_sky_circle_coordinate_factor = 0.5 / pi;

uniform sampler1D sky_circle;

layout (location = 0) out vec4 fluence;

void main()
{
	ivec2 output_texel_position = ivec2(gl_FragCoord.xy);

	int direction_id = X(output_texel_position) & direction_mask;
	float lower_direction_y = float((direction_id << 1) - cascade_power_of_two);
	float lower_direction_slope = lower_direction_y * cascade_power_of_two_inverse;
	float lower_angle = atan(lower_direction_slope);
	float upper_angle = atan(lower_direction_slope + double_cascade_power_of_two_inverse);
	float full_angular_size = upper_angle - lower_angle;
	float step_angular_size = full_angular_size * angular_step_fraction;
	
	float angle = lower_angle + step_angular_size * 0.5;
	#if DIRECTION == EAST_DIRECTION
	#elif DIRECTION == NORTH_DIRECTION
		angle = half_pi + angle;
	#elif DIRECTION == WEST_DIRECTION
		angle = pi - angle;
	#elif DIRECTION == SOUTH_DIRECTION
		angle = one_and_a_half_pi - angle;
	#endif
	fluence = vec4(0.0);
	for (int i = 0; i < angular_step_count; ++i)
	{
		// TODO: angle_to_sky_circle_coordinate_factor can be merged into angle and step_angular_size
		float sky_circle_coordinate = angle * angle_to_sky_circle_coordinate_factor;
		fluence += texture(sky_circle, sky_circle_coordinate) * step_angular_size;

		angle += step_angular_size;
	}
}
