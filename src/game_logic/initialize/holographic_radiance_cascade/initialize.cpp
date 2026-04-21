#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void initialize(game_environment::Environment& environment)
	{
		std::cout << "Initialize Holographic Radiance Cascades." << std::endl;

		::game_logic::holographic_radiance_cascades::enable(environment);
		environment.state.holographic_radiance_cascades.visible_source_layer = 0u;

		environment.state.holographic_radiance_cascades.source_width = environment.state.glfw.framebuffer_width;
		environment.state.holographic_radiance_cascades.source_height = environment.state.glfw.framebuffer_height;

		environment.state.holographic_radiance_cascades.probe_grid_width = 4u;//1024u;
		environment.state.holographic_radiance_cascades.probe_grid_height = 4u;//512u;

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

		environment.state.holographic_radiance_cascades.horizontal_trace_rays_cascade_count = 2u;
		environment.state.holographic_radiance_cascades.vertical_trace_rays_cascade_count = 2u;
		
		environment.state.holographic_radiance_cascades.horizontal_trace_rays_base_sample_count = 1u;
		environment.state.holographic_radiance_cascades.vertical_trace_rays_base_sample_count = 1u;

		// TODO: Handle ray trace cascade counts being larger than cascade counts.
		environment.state.holographic_radiance_cascades.horizontal_merge_rays_cascade_count =
			environment.state.holographic_radiance_cascades.horizontal_cascade_count -
			environment.state.holographic_radiance_cascades.horizontal_trace_rays_cascade_count;
		environment.state.holographic_radiance_cascades.vertical_merge_rays_cascade_count =
			environment.state.holographic_radiance_cascades.vertical_cascade_count -
			environment.state.holographic_radiance_cascades.vertical_trace_rays_cascade_count;

		environment.state.holographic_radiance_cascades.horizontal_skycircle_sample_count = 1u;
		environment.state.holographic_radiance_cascades.vertical_skycircle_sample_count = 1u;

		environment.state.holographic_radiance_cascades.configuration.merge_rays_stride =
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.merge_rays_data.block_state.buffer_data_size);
		environment.state.holographic_radiance_cascades.configuration.merge_fluence_stride =
			::game_logic::binding_util::align_uniform_block_size(environment, environment.state.layouts.merge_fluence_data.block_state.buffer_data_size);

		allocate(environment);
	}
}
