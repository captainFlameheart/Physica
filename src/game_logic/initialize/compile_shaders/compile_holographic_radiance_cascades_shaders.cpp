#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "game_state/initialize/compile_shaders/environment/include.h"
#include "util/shader/shader.h"
#include "game_logic/shader_util/include.h"
#include "game_logic/holographic_radiance_cascades/include.h"

namespace game_logic::initialize::compile_shaders
{
	void compile_holographic_radiance_cascades_shaders
	(
		game_environment::Environment& environment, ::game_state::initialize::compile_shaders::environment::Environment compile_environment
	)
	{
		for (GLuint bidirection{ 0u }; bidirection < 2u; ++bidirection)
		{
			GLuint cascade_count{ environment.state.holographic_radiance_cascades.cascade_counts[bidirection] };
			GLuint angular_step_count{ environment.state.holographic_radiance_cascades.skycircle_sample_counts[bidirection] };

			for (GLuint direction{ 0u }; direction < 2u; ++direction)
			{
				GLuint flattened_direction{ ::game_state::holographic_radiance_cascades::directions::flatten[bidirection][direction] };
				{	// Merge rays.
					std::string merge_rays_parameter_definitions
					{
						"#define DIRECTION " + std::to_string(flattened_direction) + "\n"
					};
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.vertex_shader,
						compile_environment.readonly_prefix_source,
						merge_rays_parameter_definitions,
						::util::shader::file_to_string("blocks/uniform/Merge_Rays_Data"),
						::util::shader::file_to_string("draw/holographic_radiance_cascades/merge_rays/merge_rays.vert")
					);
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.fragment_shader,
						compile_environment.readonly_prefix_source,
						merge_rays_parameter_definitions,
						::util::shader::file_to_string("blocks/uniform/Merge_Rays_Data"),
						::util::shader::file_to_string("draw/holographic_radiance_cascades/merge_rays/merge_rays.frag")
					);

					environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_merge_rays[bidirection][direction]] = ::util::shader::create_program
					(
						compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
					);
				}
				{	// Gather fluence from skycircle.
					std::string gather_fluence_from_skycircle_parameter_definitions
					{
						"#define DIRECTION " + std::to_string(flattened_direction) + "\n"
						"const int cascade = " + std::to_string(cascade_count) + ";\n"
						"const int angular_step_count = " + std::to_string(angular_step_count) + ";\n"
					};
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.vertex_shader,
						compile_environment.readonly_prefix_source,
						gather_fluence_from_skycircle_parameter_definitions,
						::util::shader::file_to_string("draw/holographic_radiance_cascades/skycircle/gather_fluence/gather_fluence.vert")
					);
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.fragment_shader,
						compile_environment.readonly_prefix_source,
						gather_fluence_from_skycircle_parameter_definitions,
						::util::shader::file_to_string("draw/holographic_radiance_cascades/skycircle/gather_fluence/gather_fluence.frag")
					);

					environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_gather_fluence_from_sky_circle[bidirection][direction]] = ::util::shader::create_program
					(
						compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
					);
				}
				{	// Merge fluence.
					std::string merge_fluence_parameter_definitions
					{
						"#define DIRECTION " + std::to_string(flattened_direction) + "\n"
						"#define COLLAPSE_DISTANCE_CONES " + std::to_string(1u) + "\n"
					};
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.vertex_shader,
						compile_environment.readonly_prefix_source,
						merge_fluence_parameter_definitions,
						::util::shader::file_to_string("blocks/uniform/Merge_Fluence_Data"),
						::util::shader::file_to_string("draw/holographic_radiance_cascades/merge_fluence/merge_fluence.vert")
					);
					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.fragment_shader,
						compile_environment.readonly_prefix_source,
						merge_fluence_parameter_definitions,
						::util::shader::file_to_string("blocks/uniform/Merge_Fluence_Data"),
						::util::shader::file_to_string("draw/holographic_radiance_cascades/merge_fluence/merge_fluence.frag")
					);

					environment.state.shaders[::game_state::shader_indices::draw::holographic_radiance_cascades::flatten_merge_fluence[bidirection][direction]] = ::util::shader::create_program
					(
						compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
					);
				}
			}
		}

		enum zoom_modes : GLuint
		{
			none,
			zoomed_out
		};
		zoom_modes zoom_mode{ none };

		GLuint source_width{ environment.state.holographic_radiance_cascades.source_width };
		GLuint source_height{ environment.state.holographic_radiance_cascades.source_height };

		GLuint probe_grid_width{ environment.state.holographic_radiance_cascades.probe_grid_width };
		GLuint probe_grid_height{ environment.state.holographic_radiance_cascades.probe_grid_height };

		std::string apply_fluence_parameter_definitions
		{
			"#define DEFAULT_ZOOM_MODE " + std::to_string(static_cast<GLuint>(none)) + "\n"
			"#define ZOOMED_OUT_ZOOM_MODE " + std::to_string(static_cast<GLuint>(zoomed_out)) + "\n"
			"#define ZOOM_MODE " + std::to_string(zoom_mode) + "\n"
			
			"const vec2 source_sample_point_to_probe_grid_point_factor = vec2(" +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_source_sample_point_to_probe_grid_point_factor(source_width)) + ", " +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_source_sample_point_to_probe_grid_point_factor(source_height)) +
			");\n"
			"const vec2 source_sample_point_to_probe_grid_point_bias = vec2(" +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_source_sample_point_to_probe_grid_point_bias(source_width)) + ", " +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_source_sample_point_to_probe_grid_point_bias(source_height)) +
			");\n"

			"const vec2 probe_grid_point_to_fluence_sample_point_factor = vec2(" +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_fluence_sample_point_factor(probe_grid_width)) + ", " +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_fluence_sample_point_factor(probe_grid_height)) +
			");\n"
			"const vec2 probe_grid_point_to_fluence_sample_point_bias = vec2(" +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_fluence_sample_point_bias(probe_grid_width)) + ", " +
				std::to_string(::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_fluence_sample_point_bias(probe_grid_height)) +
			");\n"
		};
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.vertex_shader,
			compile_environment.readonly_prefix_source,
			apply_fluence_parameter_definitions,
			::util::shader::file_to_string("draw/holographic_radiance_cascades/apply_fluence/apply_fluence.vert")
		);
		::util::shader::set_shader_statically
		(
			compile_environment.shader_group.fragment_shader,
			compile_environment.readonly_prefix_source,
			apply_fluence_parameter_definitions,
			::util::shader::file_to_string("draw/holographic_radiance_cascades/apply_fluence/apply_fluence.frag")
		);

		environment.state.shaders[static_cast<GLuint>(::game_state::shader_indices::draw::holographic_radiance_cascades::Indices::apply_fluence)] = ::util::shader::create_program
		(
			compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
		);
	}
}
