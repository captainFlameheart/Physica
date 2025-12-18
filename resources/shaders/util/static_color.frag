layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
	albedo = COLOR;
	albedo.a = 0.2;
	emission = 1.5 * COLOR;//vec4(1.0, 0.0, 0.0, 1.0);
	emission.a = 0.0;
	absorption = vec4(10.0);//vec4(100.0);
	scattering = vec4(0.0);
}
