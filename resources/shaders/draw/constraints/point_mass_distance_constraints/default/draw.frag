layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    albedo = vec4(0.0, 1.0, 0.0, 0.2);
    emission = vec4(0.0, 1.0, 0.0, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}
