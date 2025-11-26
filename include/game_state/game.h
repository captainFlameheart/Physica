#pragma once
#include "glad_glfw.h"
#include "game_state/camera/camera.h"
#include "util/math/vector_2D.h"
#include "game_state/proximity/Tree.h"
#include "game_state/rigid_body/Triangle.h"
#include "game_state/cursor_types/cursor_type_set.h"
#include "GPU_buffers/include.h"
#include "shaders/include.h"

template <unsigned int Vertex_Index_Count>
struct Model
{
	GLuint vertex_indices[Vertex_Index_Count];
	GLfloat inverse_mass;
	GLfloat inverse_inertia;
};

namespace game_state
{
	// TODO: Organize based on access patterns 
	// (initialize -> render -> events -> tick * n -> render -> events -> tick * n -> render -> ...)
	struct Game
	{
		GLint uniform_buffer_offset_alignment;

		GLboolean framebuffer_sRGB_enabled;
		GLuint time_elapsed_query;

		cursor_types::Cursor_Type_Set cursor_types;

		camera::Camera camera;
		GLboolean point_grabbed;
		util::math::Vector_2D grabbed_point;

		// TODO: Camera buffer should be after all tick buffers
		unsigned char* camera_send_buffer;	// TODO: Remove through persistent memory mapping
		GLuint camera_buffer;
		GLint camera_buffer_size;
		GLint camera_buffer_xy_offset;
		GLint camera_buffer_angle_offset;
		GLint camera_buffer_z_offset;
		GLint camera_buffer_view_rotation_offset;
		GLint camera_buffer_view_rotation_stride;
		GLuint shader;
		GLuint vao;
		GLuint vbo;

		GPU_buffers::GPU_Buffer_Set GPU_buffers;
		shaders::Shader_Set shaders;
		proximity::Tree proximity_tree;

		GLuint test_compute_shader;

		unsigned char* changed_bounding_boxes_mapping;
		
		unsigned char* contact_mapping;

		GLsync physics_tick_results_fence;

		GLuint grabbed_triangle;
		GLuint distance_constraint_start_triangle;

		GLuint tick;
		bool physics_running;

		bool use_holographic_radiance_cascades;

		union
		{
			struct
			{
				GLuint fluid_framebuffer;
				GLuint holographic_source_framebuffer;
				GLuint angular_fluence_framebuffer;
				GLuint ray_framebuffer;
			};
			GLuint framebuffers[4u];
		};

		// IMPORTANT TODO: Use 3D textures instead of texture 2D arrays when z locality is present!
		union
		{
			struct
			{
				GLuint fluid_texture;
				union
				{
					struct
					{
						GLuint holographic_source_array_texture;
						GLuint angular_fluence_texture;
					};
					GLuint texture_2d_arrays[2u];

				};

			};
			GLuint framebuffer_textures[3u];
		};
		GLuint max_cascade_index;
		GLuint* ray_textures;

		GLuint holographic_triangle_draw_shader;

		GLuint fluid_particles_draw_shader;

		bool debug_fluid_particles_visible;
		GLuint debug_fluid_particles_draw_shader;

		GLuint fluid_draw_shader;
		GLint fluid_draw_shader_fluid_texture_uniform_location;

		GLuint triangle_draw_shader;

		bool triangle_wireframes_visible;
		GLuint triangle_wireframes_draw_shader;
		
		GLuint hovered_triangle_wireframe_draw_shader;
		GLint hovered_triangle_wireframe_hovered_triangle_uniform_location;

		bool rigid_bodies_visible;
		GLuint rigid_body_debug_rendering_shader;

		bool triangle_normals_visible;
		GLuint triangle_normal_draw_shader;
		
		bool leaf_bounding_boxes_visible;
		GLuint triangle_bounding_box_draw_shader;
		GLuint fluid_bounding_boxes_draw_shader;

		bool parent_bounding_boxes_visible;
		GLuint parent_bounding_box_draw_shader;
		GLint parent_bounding_box_uniform_location;

		bool leaf_contacts_visible;
		GLuint fluid_leaf_contacts_draw_shader;
		GLuint fluid_triangle_leaf_contacts_draw_shader;
		GLuint leaf_triangle_contact_draw_shader;

		bool contact_point_positions_visible;
		GLuint fluid_triangle_contact_points_draw_shader;
		GLuint contact_point_positions_draw_shader;

		GLuint contact_point_offsets_draw_shader;
		
		bool contact_basis_visible;
		GLuint fluid_triangle_contact_basis_draw_shader;
		GLuint contact_basis_draw_shader;
		
		bool contact_impulses_visible;
		GLuint contact_impulses_draw_shader;

		bool gravity_visible;
		GLuint gravity_grid_points_draw_shader;
		GLuint gravity_directions_draw_shader;

		GLuint gravity_sources_draw_shader;
		GLuint hovered_gravity_source_wireframe_draw_shader;
		GLuint hovered_gravity_source_wireframe_draw_shader_hovered_gravity_source_uniform_location;

		GLuint distance_constraints_draw_shader;

		GLuint distance_constraint_start_draw_shader;
		
		GLuint cursor_constraint_draw_shader;
		
		GLuint cursor_position_draw_shader;

		GLuint holographic_source_draw_shader;
		GLint holographic_source_draw_shader_source_uniform_location;
		GLint holographic_source_draw_shader_layer_uniform_location;

		union
		{
			struct
			{
				GLuint holographic_probe_grid_width;
				GLuint holographic_probe_grid_height;
			};
			GLuint holographic_probe_grid_size[2u];
		};

		GLuint holographic_probe_grid_draw_shader;
		GLint holographic_probe_grid_draw_shader_probe_grid_size_uniform_location;

		GLuint holographic_cascade_draw_shader;
		GLint holographic_cascade_draw_shader_probe_grid_size_uniform_location;
		GLuint holographic_cascade_draw_shader_cascade;
		GLint holographic_cascade_draw_shader_cascade_uniform_location;

		GLuint holographic_ray_trace_shader_count;
		GLuint* holographic_ray_trace_shaders;

		GLuint holographic_ray_extend_shader;
		GLint holographic_ray_extend_shader_shorter_rays_uniform_location;

		GLuint holographic_ray_extend_buffer;
		GLint holographic_ray_extend_buffer_size;
		GLint holographic_ray_extend_buffer_skipped_rays_below_column_offset;
		GLint holographic_ray_extend_buffer_rays_per_probe_offset;
		GLint holographic_ray_extend_buffer_g_offset;
		GLint holographic_ray_extend_buffer_f_offset;
		GLint holographic_ray_extend_buffer_lower_cascade_rays_per_probe_offset;
		GLint holographic_ray_extend_buffer_lower_cascade_skipped_rays_below_column_offset;
		GLint holographic_ray_extend_buffer_lower_cascade_max_ray_probe_column_offset;
		GLint holographic_ray_extend_buffer_lower_cascade_max_ray_probe_row_offset;

		GLuint presentation_stage;
	};
}
