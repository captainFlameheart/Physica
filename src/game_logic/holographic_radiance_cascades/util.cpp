#include "game_logic/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include <algorithm>
#include "global/include.h"

namespace game_logic::holographic_radiance_cascades
{
	void create_vector(GLuint(&vector)[2u], GLuint x, GLuint y, GLuint bidirection, GLuint orthogonal_bidirection);
	{
		vector[bidirection] = x;
		vector[orthogonal_bidirection] = y;
	}

	GLuint compute_rays_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade, GLuint orthogonal_probe_grid_length,
		GLuint bidirection, GLuint orthogonal_bidirection
	)
	{
		GLuint x{ (((probe_grid_length - 2u + cascade_power_of_two) >> cascade) - 1u) * (cascade_power_of_two + 1u) };
		GLuint y{ orthogonal_probe_grid_length };
		create_vector(size, x, y, bidirection, orthogonal_bidirection);
	}

	GLuint compute_gather_fluence_from_skycircle_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade,
		GLuint bidirection, GLuint orthogonal_bidirection
	)
	{
		GLuint x{ ((probe_grid_length - 2u + cascade_power_of_two) >> cascade) * cascade_power_of_two };
		GLuint y{ 1u };
		create_vector(size, x, y, bidirection, orthogonal_bidirection);
	}

	GLuint compute_merge_fluence_inner_viewport_size
	(
		GLuint(&size)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade, GLuint orthogonal_probe_grid_length,
		GLuint bidirection, GLuint orthogonal_bidirection
	)
	{
		GLuint x{ ((probe_grid_length - 2u + cascade_power_of_two) >> cascade) * cascade_power_of_two - cascade_power_of_two };
		GLuint y{ orthogonal_probe_grid_length };
		create_vector(size, x, y, bidirection, orthogonal_bidirection);
	}

	GLuint compute_merge_fluence_outer_viewport_start
	(
		GLuint(&start)[2u], GLuint probe_grid_length, GLuint cascade_power_of_two, GLuint cascade,
		GLuint bidirection, GLuint orthogonal_bidirection
	)
	{
		GLuint x{ ((probe_grid_length - 2u + cascade_power_of_two) >> cascade) * cascade_power_of_two - cascade_power_of_two };
		GLuint y{ 0u };
		create_vector(start, x, y, bidirection, orthogonal_bidirection);
	}

	GLuint compute_merge_fluence_outer_viewport_size
	(
		GLuint(&size)[2u], GLuint cascade_power_of_two,
		GLuint bidirection, GLuint orthogonal_bidirection
	)
	{
		GLuint x{ cascade_power_of_two };
		GLuint y{ 1u };
		create_vector(size, x, y, bidirection, orthogonal_bidirection);
	}

	void enable(game_environment::Environment& environment)
	{
		environment.state.holographic_radiance_cascades.enabled = true;
	}

	void disable(game_environment::Environment& environment)
	{
		environment.state.holographic_radiance_cascades.enabled = false;
	}

	void toggle(game_environment::Environment& environment)
	{
		if (environment.state.holographic_radiance_cascades.enabled)
		{
			disable(environment);
		}
		else
		{
			enable(environment);
		}
	}
}
