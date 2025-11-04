#pragma once
#include <GLFW/glfw3.h>
#include "game_state/rigid_body/Triangle.h"

namespace game_state::GPU_buffers::rigid_bodies::triangles::contact_surfaces
{
	struct Buffer
	{
		// TODO: Consider using std::430 layout to improve runtime performance
		GLuint buffer;
		GLint size;

		GLint contact_surfaces_bodies_offset;
		GLint contact_surfaces_bodies_stride;

		GLint contact_surfaces_contact_point_position_0_offsets_offset;
		GLint contact_surfaces_contact_point_position_0_offsets_stride;
		GLint contact_surfaces_contact_point_position_1_offsets_offset;
		GLint contact_surfaces_contact_point_position_1_offsets_stride;

		GLint contact_surfaces_tangent_offset;
		GLint contact_surfaces_contact_point_tangent_0_mass_offset;
		GLint contact_surfaces_contact_point_tangent_0_impulse_offset;
		GLint contact_surfaces_contact_point_tangent_1_mass_offset;
		GLint contact_surfaces_contact_point_tangent_1_impulse_offset;

		GLint contact_surfaces_contact_point_normal_0_target_velocity_offset;
		GLint contact_surfaces_contact_point_normal_0_mass_offset;
		GLint contact_surfaces_contact_point_normal_0_impulse_offset;
		GLint contact_surfaces_contact_point_normal_1_target_velocity_offset;
		GLint contact_surfaces_contact_point_normal_1_mass_offset;
		GLint contact_surfaces_contact_point_normal_1_impulse_offset;

		GLint contact_surfaces_offset;
		GLint contact_surfaces_stride;
	};
}
