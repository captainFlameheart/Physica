//uniform vec4 color;

noperspective in float offset;

layout(location = 0) out vec4 radiance;

void main()
{
	const vec4 color = vec4(1.0, 1.0, 1.0, 0.0) * 2.0;

	float brightness = 1.0 - abs(offset - 0.5) * 2.0;
	brightness *= brightness;
	radiance = color * brightness;

	//radiance = vec4(1.0, 1.0, 1.0, 0.0);
}
