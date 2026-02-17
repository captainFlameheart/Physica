#pragma once
#include "glad_glfw.h"

namespace game_state::shader_indices
{
	constexpr GLuint base{ 0u };

	namespace initialize
	{
		constexpr GLuint base{ ::game_state::shader_indices::base };

		enum struct Indices : GLuint
		{
			initialize = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace tick
	{
		constexpr GLuint base{ ::game_state::shader_indices::initialize::end };

		namespace update_counts
		{
			constexpr GLuint base{ ::game_state::shader_indices::tick::base };

			enum struct Indices : GLuint
			{
				update_counts = base,

				end
			};

			constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
			constexpr GLuint count{ end - base };
		}
		namespace process_entities
		{
			constexpr GLuint base{ ::game_state::shader_indices::tick::update_counts::end };
	
			namespace bodies
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::base };

				enum struct Indices : GLuint
				{
					point_masses = base,
					rigid_bodies,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bodies::end };

				enum struct Indices : GLuint
				{
					point_mass_distance_constraints = base,
					point_mass_uniform_force_constraints,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}
	
			constexpr GLuint end{ constraints::end };
			constexpr GLuint count{ end - base };
		}

		constexpr GLuint end{ process_entities::end };
		constexpr GLuint count{ end - base };
	}

	namespace draw
	{
		constexpr GLuint base{ ::game_state::shader_indices::tick::end };
		
		namespace update_counts
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::base };

			enum struct Indices : GLuint
			{
				update_counts = base,

				end
			};

			constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
			constexpr GLuint count{ end - base };
		}

		namespace entities
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::update_counts::end };

			namespace bodies
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::base };

				enum struct Indices : GLuint
				{
					point_masses = base,
					point_mass_velocities,
					point_mass_forces,
					rigid_bodies,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::bodies::end };

				enum struct Indices : GLuint
				{
					point_mass_distance_constraints = base,
					point_mass_uniform_force_constraints,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			constexpr GLuint end{ constraints::end };
			constexpr GLuint count{ end - base };
		}

		constexpr GLuint end{ entities::end };
		constexpr GLuint count{ end - base };
	}

	constexpr GLuint end{ draw::end };
	constexpr GLuint count{ end - base };
}
