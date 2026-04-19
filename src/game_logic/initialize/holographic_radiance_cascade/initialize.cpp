#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void initialize(game_environment::Environment& environment)
	{
		std::cout << "Initialize Holographic Radiance Cascades." << std::endl;

		environment.state.holographic_radiance_cascades.enabled = true;
		environment.state.holographic_radiance_cascades.visible_source_layer = 0u;

		environment.state.holographic_radiance_cascades.source_width = environment.state.glfw.framebuffer_width;
		environment.state.holographic_radiance_cascades.source_height = environment.state.glfw.framebuffer_height;

		environment.state.holographic_radiance_cascades.probe_grid_width = 1024u;
		environment.state.holographic_radiance_cascades.probe_grid_height = 512u;

		environment.state.holographic_radiance_cascades.horizontal_cascade_count = ceil_log2_n_minus_1(environment.state.holographic_radiance_cascades.probe_grid_width);
		environment.state.holographic_radiance_cascades.vertical_cascade_count = ceil_log2_n_minus_1(environment.state.holographic_radiance_cascades.probe_grid_height);

		environment.state.holographic_radiance_cascades.cascade_count = std::max
		(
			environment.state.holographic_radiance_cascades.horizontal_cascade_count,
			environment.state.holographic_radiance_cascades.vertical_cascade_count
		);

		environment.state.holographic_radiance_cascades.skycircle_length = 1024u;

		environment.state.holographic_radiance_cascades.horizontal_skycircle_sample_count = 1u;
		environment.state.holographic_radiance_cascades.vertical_skycircle_sample_count = 1u;

		environment.state.holographic_radiance_cascades.horizontal_ray_trace_cascade_count = 2u;
		environment.state.holographic_radiance_cascades.vertical_ray_trace_cascade_count = 2u;
		
		environment.state.holographic_radiance_cascades.horizontal_ray_trace_base_sample_count = 1u;
		environment.state.holographic_radiance_cascades.vertical_ray_trace_base_sample_count = 1u;

		allocate(environment);
	}
}
