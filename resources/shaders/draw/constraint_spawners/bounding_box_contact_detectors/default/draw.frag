in vec4 color;

layout(location = albedo_layer) out vec4 albedo;
layout(location = emission_layer) out vec4 emission;
layout(location = attenuation_layer) out vec4 absorption;
layout(location = scattering_layer) out vec4 scattering;

void main()
{
    albedo = color;
    emission = vec4(color.rgb * 0.2, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}
