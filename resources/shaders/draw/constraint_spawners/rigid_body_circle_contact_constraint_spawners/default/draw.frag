in vec4 color;

layout(location = albedo_layer) out vec4 albedo;
layout(location = emission_layer) out vec4 emission;
layout(location = attenuation_layer) out vec4 absorption;
layout(location = scattering_layer) out vec4 scattering;

void main()
{
    albedo = color;
    emission = vec4(albedo.rgb * default_emission_scalar, 0.0);
    absorption = default_attenuation;
    scattering = default_scattering;
}
