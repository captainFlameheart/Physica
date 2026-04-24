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

	namespace on_key_event
	{
		constexpr GLuint base{ ::game_state::shader_indices::initialize::end };

		enum struct Indices : GLuint
		{
			on_key_event = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace on_mouse_button_event
	{
		constexpr GLuint base{ ::game_state::shader_indices::on_key_event::end };

		enum struct Indices : GLuint
		{
			on_mouse_button_event = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace on_scroll_event
	{
		constexpr GLuint base{ ::game_state::shader_indices::on_mouse_button_event::end };

		enum struct Indices : GLuint
		{
			on_scroll_event = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace on_events_polled
	{
		constexpr GLuint base{ ::game_state::shader_indices::on_scroll_event::end };

		enum struct Indices : GLuint
		{
			on_events_polled = base,

			end
		};

		constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
		constexpr GLuint count{ end - base };
	}

	namespace tick
	{
		constexpr GLuint base{ ::game_state::shader_indices::on_events_polled::end };

		namespace update_counts
		{
			constexpr GLuint base{ ::game_state::shader_indices::tick::base };

			enum struct Indices : GLuint
			{
				update_counts = base,
				on_update_counts,

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

			namespace skycircle_elements
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bodies::end };

				enum struct Indices : GLuint
				{
					default_elements = base,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace bounding_volume_hierarchy
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::skycircle_elements::end };

				namespace send_to_CPU
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::base };

					enum struct Indices : GLuint
					{
						send_to_CPU = base,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace leafs
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::send_to_CPU::end};

					enum struct Indices : GLuint
					{
						rigid_body_circles = base,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace initialize_inner_bounding_box_traversal
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::leafs::end };

					enum struct Indices : GLuint
					{
						find_height_changes = base,
						migrate_inner_bounding_boxes,
						initialize_inner_bounding_box_traversal,
						set_commands,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace inner_bounding_boxes
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::initialize_inner_bounding_box_traversal::end };

					enum struct Indices : GLuint
					{
						inner_bounding_boxes = base,
						increment_height_level,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace swap_leaf_bounding_box_buffers
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::inner_bounding_boxes::end };

					enum struct Indices : GLuint
					{
						swap_leaf_bounding_box_buffers = base,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				constexpr GLuint end{ static_cast<GLuint>(swap_leaf_bounding_box_buffers::end) };
				constexpr GLuint count{ end - base };
			}

			namespace pre_constraint_spawners
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::bounding_volume_hierarchy::end };

				namespace commit_counts
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::base };

					enum struct Indices : GLuint
					{
						commit_counts = base,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace plan_compaction
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::commit_counts::end };

					enum struct Indices : GLuint
					{
						plan_rigid_body_circle_contact_constraint_spawner_compaction = base,
						
						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace clear_deaths
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::plan_compaction::end };

					enum struct Indices : GLuint
					{
						clear_bounding_box_contact_detector_deaths = base,
						clear_rigid_body_circle_contact_constraint_spawner_deaths,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace perform_compaction
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::clear_deaths::end };

					enum struct Indices : GLuint
					{
						perform_bounding_box_contact_detector_compaction = base,
						perform_rigid_body_circle_contact_constraint_spawner_compaction,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				constexpr GLuint end{ static_cast<GLuint>(perform_compaction::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraint_spawners
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraint_spawners::end };

				enum struct Indices : GLuint
				{
					bounding_box_contact_detectors = base,
					rigid_body_circle_contact_constraint_spawners,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace pre_constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::constraint_spawners::end };

				namespace commit_counts
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraints::base };

					enum struct Indices : GLuint
					{
						commit_counts = base,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace plan_compaction
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraints::commit_counts::end };

					enum struct Indices : GLuint
					{
						plan_point_mass_distance_constraint_compaction = base,
						plan_rigid_body_circle_contact_constraint_compaction,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace clear_deaths
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraints::plan_compaction::end };

					enum struct Indices : GLuint
					{
						clear_point_mass_distance_constraint_deaths = base,
						clear_rigid_body_circle_contact_constraint_deaths,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				namespace perform_compaction
				{
					constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraints::clear_deaths::end };

					enum struct Indices : GLuint
					{
						perform_point_mass_distance_constraint_compaction = base,
						perform_rigid_body_circle_contact_constraint_compaction,

						end
					};

					constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
					constexpr GLuint count{ end - base };
				}

				constexpr GLuint end{ static_cast<GLuint>(perform_compaction::end) };
				constexpr GLuint count{ end - base };
			}

			namespace on_tick_constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::pre_constraints::end };

				enum struct Indices : GLuint
				{
					on_tick_constraints = base,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::tick::process_entities::on_tick_constraints::end };

				enum struct Indices : GLuint
				{
					point_mass_distance_constraints = base,
					rigid_body_circle_contact_constraints,

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
		
		namespace highlight_hovered_objects
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::base };

			enum struct Indices : GLuint
			{
				clear_hover_highlight_detector_counts = base,
				find_highlighted_hovered_leafs,
				commit_hover_highlight_detector_counts,

				detect_rigid_body_circle_hover_highlights,
				detect_rigid_body_triangle_hover_highlights,

				end
			};

			constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
			constexpr GLuint count{ end - base };
		}

		namespace update_counts
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::highlight_hovered_objects::end };

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
					rigid_body_triangles,
					rigid_body_circles,
					rigid_bodies,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraint_spawners
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::bodies::end };

				enum struct Indices : GLuint
				{
					bounding_box_contact_detectors = base,
					rigid_body_circle_contact_constraint_spawners,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace constraints
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::constraint_spawners::end };

				enum struct Indices : GLuint
				{
					point_mass_distance_constraints = base,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace bounding_volume_hierarchy
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::constraints::end };

				enum struct Indices : GLuint
				{
					rigid_body_circle_bounding_boxes = base,
					inner_bounding_boxes,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			namespace skycircle_elements
			{
				constexpr GLuint base{ ::game_state::shader_indices::draw::entities::bounding_volume_hierarchy::end };

				enum struct Indices : GLuint
				{
					default_elements = base,

					end
				};

				constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
				constexpr GLuint count{ end - base };
			}

			constexpr GLuint end{ skycircle_elements::end };
			constexpr GLuint count{ end - base };
		}

		namespace cursor
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::entities::end };

			enum struct Indices : GLuint
			{
				cursor = base,

				end
			};

			constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
			constexpr GLuint count{ end - base };
		}

		namespace holographic_radiance_cascades
		{
			constexpr GLuint base{ ::game_state::shader_indices::draw::cursor::end };

			enum struct Indices : GLuint
			{
				source_layer = base,
				skycircle_ring,
				
				east_merge_rays,
				east_gather_fluence_from_skycircle,
				east_merge_fluence,
				
				west_merge_rays,
				west_gather_fluence_from_skycircle,
				west_merge_fluence,
				
				north_merge_rays,
				north_gather_fluence_from_skycircle,
				north_merge_fluence,
				
				south_merge_rays,
				south_gather_fluence_from_skycircle,
				south_merge_fluence,

				apply_fluence,

				end
			};

			constexpr GLuint flatten_merge_rays[2u][2u]
			{
				{ static_cast<GLuint>(Indices::east_merge_rays), static_cast<GLuint>(Indices::west_merge_rays) },
				{ static_cast<GLuint>(Indices::north_merge_rays), static_cast<GLuint>(Indices::south_merge_rays) }
			};
			constexpr GLuint flatten_gather_fluence_from_skycircle[2u][2u]
			{
				{ static_cast<GLuint>(Indices::east_gather_fluence_from_skycircle), static_cast<GLuint>(Indices::west_gather_fluence_from_skycircle) },
				{ static_cast<GLuint>(Indices::north_gather_fluence_from_skycircle), static_cast<GLuint>(Indices::south_gather_fluence_from_skycircle) }
			};
			constexpr GLuint flatten_merge_fluence[2u][2u]
			{
				{ static_cast<GLuint>(Indices::east_merge_fluence), static_cast<GLuint>(Indices::west_merge_fluence) },
				{ static_cast<GLuint>(Indices::north_merge_fluence), static_cast<GLuint>(Indices::south_merge_fluence) }
			};

			constexpr GLuint end{ static_cast<GLuint>(Indices::end) };
			constexpr GLuint count{ end - base };
		}

		constexpr GLuint end{ holographic_radiance_cascades::end };
		constexpr GLuint count{ end - base };
	}

	constexpr GLuint end{ draw::end };
	constexpr GLuint count{ end - base };
}
