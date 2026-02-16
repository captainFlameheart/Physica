#include "glad_glfw.h"
#include <array>

enum struct Process_Bodies_Shader_Indices: GLuint
{
	point_masses,
	rigid_bodies,
	
	count
};

enum struct Process_Constraints_Shader_Indices : GLuint
{
	point_mass_distance_constraints,
	point_mass_uniform_constraints,

	count
};

constexpr GLuint process_entites_count
{
	static_cast<GLuint>(Process_Bodies_Shader_Indices::count) + static_cast<GLuint>(Process_Constraints_Shader_Indices::count)
};

constexpr std::array<GLuint, process_entites_count> initialize_local_sizes()
{
	std::array<GLuint, process_entites_count> local_sizes;
	local_sizes[static_cast<GLuint>(Process_Bodies_Shader_Indices::point_masses)] = 256u;
	local_sizes[static_cast<GLuint>(Process_Bodies_Shader_Indices::rigid_bodies)] = 256u;
	local_sizes[static_cast<GLuint>(Process_Bodies_Shader_Indices::count) + static_cast<GLuint>(Process_Constraints_Shader_Indices::point_mass_distance_constraints)] = 256u;
	local_sizes[static_cast<GLuint>(Process_Bodies_Shader_Indices::count) + static_cast<GLuint>(Process_Constraints_Shader_Indices::point_mass_uniform_constraints)] = 256u;
	return local_sizes;
}

constexpr std::array<GLuint, process_entites_count> local_sizes{ initialize_local_sizes() };

