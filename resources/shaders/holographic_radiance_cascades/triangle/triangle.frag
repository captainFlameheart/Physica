struct Material
{
	vec4 albedo;
	vec4 emission;
	vec4 absorption;
	vec4 scattering;
};

layout(shared, binding = MATERIALS_BINDING) restrict readonly
buffer Materials
{
	Material materials[MAX_MATERIAL_COUNT];
} materials;

layout(shared, binding = MATERIAL_INDICES_BINDING) restrict readonly
buffer Material_Indices
{
	uint material_indices[MAX_TRIANGLE_COUNT];
} material_indices;

in vec4 material_albedo;
in vec4 material_emission;
in vec4 material_absorption;
in vec4 material_scattering;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
	albedo = material_albedo;
	emission = material_emission;
	absorption = material_absorption;
	scattering = material_scattering;
}
