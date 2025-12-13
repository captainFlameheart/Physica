noperspective in float offset;

layout(location = 0) out vec4 radiance;

void main()
{
	float brightness = 1.0 - abs(offset - 0.5) * 2.0;
	brightness *= brightness;
	radiance = vec4(brightness, brightness, brightness, 0.0);
}
