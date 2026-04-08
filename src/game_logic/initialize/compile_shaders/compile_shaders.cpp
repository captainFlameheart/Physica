#include "game_logic/initialize/compile_shaders/include.h"
#include "glad_glfw.h"
#include "game_environment/environment.h"
#include "util/shader/shader.h"
#include "game_logic/initialize/compile_shaders/environment/include.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders
{
	void compile_shaders(game_environment::Environment& environment)
	{
		::game_state::initialize::compile_shaders::environment::Environment compile_environment;
		environment::initialize(environment, compile_environment);

		compile_initialize(environment, compile_environment);
		
		compile_on_key_event(environment, compile_environment);

		compile_update_tick_counts(environment, compile_environment);
		compile_on_update_tick_counts(environment, compile_environment);
		
		compile_process_point_masses(environment, compile_environment);
		compile_tick_rigid_bodies(environment, compile_environment);

		compile_send_bounding_volume_hierarchy_to_CPU(environment, compile_environment);

		compile_tick_rigid_body_circles(environment, compile_environment);
		
		compile_find_inner_bounding_box_height_changes(environment, compile_environment);
		compile_migrate_inner_bounding_boxes(environment, compile_environment);
		compile_initialize_inner_bounding_box_traversal(environment, compile_environment);
		compile_set_inner_bounding_box_tick_commands(environment, compile_environment);

		compile_tick_inner_bounding_boxes(environment, compile_environment);
		compile_increment_inner_bounding_box_height_level(environment, compile_environment);

		compile_swap_leaf_bounding_box_buffers(environment, compile_environment);

		compile_commit_constraint_spawner_counts(environment, compile_environment);
		
		compile_clear_rigid_body_circle_contact_constraint_spawner_deaths(environment, compile_environment);

		compile_perform_rigid_body_circle_contact_constraint_spawner_compaction(environment, compile_environment);

		compile_tick_rigid_body_circle_contact_constraint_spawners(environment, compile_environment);

		compile_commit_constraint_counts(environment, compile_environment);

		compile_clear_rigid_body_circle_contact_constraint_deaths(environment, compile_environment);

		compile_perform_rigid_body_circle_contact_constraint_compaction(environment, compile_environment);

		compile_on_tick_constraints(environment, compile_environment);
		compile_tick_point_mass_distance_constraints(environment, compile_environment);
		compile_tick_rigid_body_circle_contact_constraints(environment, compile_environment);
		
		compile_update_draw_counts(environment, compile_environment);

		compile_draw_point_masses(environment, compile_environment);
		compile_draw_point_mass_velocities(environment, compile_environment);
		compile_draw_point_mass_forces(environment, compile_environment);

		compile_draw_rigid_body_circle_contact_constraint_spawners(environment, compile_environment);

		compile_draw_rigid_body_circles(environment, compile_environment);
		compile_draw_rigid_body_triangles(environment, compile_environment);
		compile_draw_rigid_bodies(environment, compile_environment);

		compile_draw_point_mass_distance_constraints(environment, compile_environment);

		compile_draw_rigid_body_circle_bounding_boxes(environment, compile_environment);
		compile_draw_inner_bounding_boxes(environment, compile_environment);

		environment::free(environment, compile_environment);
	}
}
