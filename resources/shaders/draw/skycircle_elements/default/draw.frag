flat in	vec4 color;
flat in float point_distance;
noperspective in float offset;

layout(location = 0, index = 0) out vec4 radiance;
layout(location = 0, index = 1) out vec4 blend_factor;

void main()
{
	radiance = color;
	float brightness = 1.0 - abs(offset - 0.5) * 2.0;
	brightness *= point_distance * 10.0;
	blend_factor = vec4(min(brightness, 1.0));
}
