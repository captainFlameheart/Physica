layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    albedo = vec4(0.0, 0.5, 1.0, default_reflectivity);
    emission = vec4(albedo.rgb * default_emission_scalar, 0.0);
    absorption = default_attenuation;
    scattering = default_scattering;
}
