in vec2 offset;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    if (dot(offset, offset) > 1.0)
    {
        discard;
    }

    albedo = vec4(0.0, 1.0, 1.0, default_reflectivity);
    emission = vec4(albedo.rgb * default_emission_scalar, default_alpha_emission);
    absorption = default_attenuation;
    scattering = default_scattering;
}
