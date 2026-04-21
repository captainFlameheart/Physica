#include "game_logic/initialize/holographic_radiance_cascades/include.h"
#include "glad_glfw.h"
#include "game_environment/Environment.h"
#include <array>

namespace game_logic::initialize::holographic_radiance_cascades
{
	void allocate_source_image(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.source_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.source_framebuffer);

		// TODO: Allow for different internal formats.
		glTextureStorage3D
		(
			environment.state.holographic_radiance_cascades.source_texture, 1u, GL_RGBA32F,
			environment.state.glfw.framebuffer_width, environment.state.glfw.framebuffer_height,
			::game_state::holographic_radiance_cascades::source_layers::count
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR
		);

		GLfloat border_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_BORDER_COLOR, border_color
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.source_texture,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER
		);

		GLenum source_draw_buffers[::game_state::holographic_radiance_cascades::source_layers::count];
		for (GLuint layer{ 0u }; layer < ::game_state::holographic_radiance_cascades::source_layers::count; ++layer)
		{
			GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
			glNamedFramebufferTextureLayer
			(
				environment.state.holographic_radiance_cascades.source_framebuffer,
				color_attachment,
				environment.state.holographic_radiance_cascades.source_texture, 0, layer
			);
			source_draw_buffers[layer] = color_attachment;
		}
		glNamedFramebufferDrawBuffers(environment.state.holographic_radiance_cascades.source_framebuffer, std::size(source_draw_buffers), source_draw_buffers);

		GLenum source_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.source_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (source_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades source framebuffer invalid, status: " << source_framebuffer_status << std::endl;
		}

		glBindTextureUnit(::game_state::texture_units::source_image, environment.state.holographic_radiance_cascades.source_texture);
	}

	void allocate_skycircle(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_1D, 1u, &environment.state.holographic_radiance_cascades.skycircle_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.skycircle_framebuffer);

		// TODO: Allow for different internal formats.
		glTextureStorage1D
		(
			environment.state.holographic_radiance_cascades.skycircle_texture, 1u, GL_RGBA32F,
			environment.state.holographic_radiance_cascades.skycircle_length
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.skycircle_texture,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.skycircle_texture,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.skycircle_texture,
			GL_TEXTURE_WRAP_S, GL_REPEAT
		);

		glNamedFramebufferTexture
		(
			environment.state.holographic_radiance_cascades.skycircle_framebuffer,
			GL_COLOR_ATTACHMENT0,
			environment.state.holographic_radiance_cascades.skycircle_texture, 0
		);
		glNamedFramebufferDrawBuffer(environment.state.holographic_radiance_cascades.skycircle_framebuffer, GL_COLOR_ATTACHMENT0);

		GLenum skycircle_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.skycircle_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (skycircle_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades skycircle framebuffer invalid, status: " << skycircle_framebuffer_status << std::endl;
		}

		glBindTextureUnit(::game_state::texture_units::skycircle, environment.state.holographic_radiance_cascades.skycircle_texture);
	}

	void allocate_rays(game_environment::Environment& environment)
	{
		environment.state.holographic_radiance_cascades.ray_textures = new GLuint[environment.state.holographic_radiance_cascades.cascade_count];
		environment.state.holographic_radiance_cascades.ray_framebuffers = new GLuint[environment.state.holographic_radiance_cascades.cascade_count];

		glCreateTextures
		(
			GL_TEXTURE_2D_ARRAY,
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_textures
		);
		glCreateFramebuffers
		(
			environment.state.holographic_radiance_cascades.cascade_count,
			environment.state.holographic_radiance_cascades.ray_framebuffers
		);

		for (GLuint cascade{ 0u }; cascade < environment.state.holographic_radiance_cascades.cascade_count; ++cascade)
		{
			GLuint cascade_power_of_two{ 1u << cascade };
			GLuint ray_texture_width
			{
				::game_logic::holographic_radiance_cascades::compute_ray_texture_length
				(
					environment.state.holographic_radiance_cascades.probe_grid_width,
					cascade,
					environment.state.holographic_radiance_cascades.vertical_cascade_count,
					cascade_power_of_two
				)
			};
			GLuint ray_texture_height
			{
				::game_logic::holographic_radiance_cascades::compute_ray_texture_length
				(
					environment.state.holographic_radiance_cascades.probe_grid_height,
					cascade,
					environment.state.holographic_radiance_cascades.horizontal_cascade_count,
					cascade_power_of_two
				)
			};
			// TODO: Allow radiance and transmittance layers to be merged.
			glTextureStorage3D
			(
				environment.state.holographic_radiance_cascades.ray_textures[cascade], 1u, GL_RGBA32F,
				ray_texture_width, ray_texture_height,
				::game_state::holographic_radiance_cascades::ray_layers::count
			);

			GLuint ray_draw_buffers[::game_state::holographic_radiance_cascades::ray_layers::count];
			for (GLuint layer{ 0u }; layer < ::game_state::holographic_radiance_cascades::ray_layers::count; ++layer)
			{
				GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
				glNamedFramebufferTextureLayer
				(
					environment.state.holographic_radiance_cascades.ray_framebuffers[cascade],
					color_attachment,
					environment.state.holographic_radiance_cascades.ray_textures[cascade], 0, layer
				);
				ray_draw_buffers[layer] = color_attachment;
			}
			glNamedFramebufferDrawBuffers(environment.state.holographic_radiance_cascades.ray_framebuffers[cascade], std::size(ray_draw_buffers), ray_draw_buffers);

			GLenum ray_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.ray_framebuffers[cascade], GL_DRAW_FRAMEBUFFER) };
			if (ray_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
			{
				std::cerr << "Holographic Radiance Cascades ray framebuffer " << cascade << " invalid, status: " << ray_framebuffer_status << std::endl;
			}
		}
	}

	void allocate_angular_fluence(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1u, &environment.state.holographic_radiance_cascades.angular_fluence_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.angular_fluence_framebuffer);

		GLuint angular_fluence_texture_width
		{
			::game_logic::holographic_radiance_cascades::compute_angular_fluence_texture_length
			(
				environment.state.holographic_radiance_cascades.horizontal_cascade_count,
				environment.state.holographic_radiance_cascades.probe_grid_width
			)
		};
		GLuint angular_fluence_texture_height
		{
			::game_logic::holographic_radiance_cascades::compute_angular_fluence_texture_length
			(
				environment.state.holographic_radiance_cascades.vertical_cascade_count,
				environment.state.holographic_radiance_cascades.probe_grid_height
			)
		};

		glTextureStorage3D
		(
			environment.state.holographic_radiance_cascades.angular_fluence_texture, 1u, GL_RGBA32F,
			angular_fluence_texture_width, angular_fluence_texture_height,
			2u
		);

		for (GLuint layer{ 0u }; layer < 2u; ++layer)
		{
			GLenum color_attachment{ GL_COLOR_ATTACHMENT0 + layer };
			glNamedFramebufferTextureLayer
			(
				environment.state.holographic_radiance_cascades.angular_fluence_framebuffer,
				color_attachment,
				environment.state.holographic_radiance_cascades.angular_fluence_texture, 0, layer
			);
		}

		GLenum angular_fluence_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.angular_fluence_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (angular_fluence_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades angular fluence framebuffer invalid, status: " << angular_fluence_framebuffer_status << std::endl;
		}

		glBindTextureUnit(::game_state::texture_units::upper_cascade_fluence, environment.state.holographic_radiance_cascades.angular_fluence_texture);
	}

	void allocate_fluence(game_environment::Environment& environment)
	{
		glCreateTextures(GL_TEXTURE_2D, 1u, &environment.state.holographic_radiance_cascades.fluence_texture);
		glCreateFramebuffers(1u, &environment.state.holographic_radiance_cascades.fluence_framebuffer);

		GLuint fluence_texture_width{ environment.state.holographic_radiance_cascades.probe_grid_width };
		GLuint fluence_texture_height{ environment.state.holographic_radiance_cascades.probe_grid_height };

		glTextureStorage2D
		(
			environment.state.holographic_radiance_cascades.fluence_texture, 1u, GL_RGBA32F,
			fluence_texture_width, fluence_texture_height
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_MIN_FILTER, GL_LINEAR
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_MAG_FILTER, GL_LINEAR
		);

		GLfloat border_color[4u]{ 0.0f, 0.0f, 0.0f, 0.0f };
		glTextureParameterfv
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_BORDER_COLOR, border_color
		);

		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER
		);
		glTextureParameteri
		(
			environment.state.holographic_radiance_cascades.fluence_texture,
			GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER
		);

		glNamedFramebufferTexture
		(
			environment.state.holographic_radiance_cascades.fluence_framebuffer,
			GL_COLOR_ATTACHMENT0,
			environment.state.holographic_radiance_cascades.fluence_texture, 0
		);
		glNamedFramebufferDrawBuffer
		(
			environment.state.holographic_radiance_cascades.fluence_framebuffer,
			GL_COLOR_ATTACHMENT0
		);

		GLenum fluence_framebuffer_status{ glCheckNamedFramebufferStatus(environment.state.holographic_radiance_cascades.fluence_framebuffer, GL_DRAW_FRAMEBUFFER) };
		if (fluence_framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Holographic Radiance Cascades fluence framebuffer invalid, status: " << fluence_framebuffer_status << std::endl;
		}

		glBindTextureUnit(::game_state::texture_units::fluence, environment.state.holographic_radiance_cascades.fluence_texture);
	}

	void allocate_configuration(game_environment::Environment& environment)
	{
		glCreateBuffers(1u, &environment.state.holographic_radiance_cascades.configuration.buffer);

		GLuint merge_rays_stride{ environment.state.holographic_radiance_cascades.configuration.merge_rays_stride };
		GLuint merge_fluence_stride{ environment.state.holographic_radiance_cascades.configuration.merge_fluence_stride };

		GLuint horizontal_merge_rays_cascade_count{ environment.state.holographic_radiance_cascades.horizontal_merge_rays_cascade_count };
		GLuint horizontal_merge_fluence_cascade_count{ environment.state.holographic_radiance_cascades.horizontal_cascade_count };

		GLuint vertical_merge_rays_cascade_count{ environment.state.holographic_radiance_cascades.vertical_merge_rays_cascade_count };
		GLuint vertical_merge_fluence_cascade_count{ environment.state.holographic_radiance_cascades.vertical_cascade_count };
		
		GLuint buffer_size
		{
			((merge_rays_stride * horizontal_merge_rays_cascade_count) + (merge_fluence_stride * horizontal_merge_fluence_cascade_count)) * 2u +
			((merge_rays_stride * vertical_merge_rays_cascade_count) + (merge_fluence_stride * vertical_merge_fluence_cascade_count)) * 2u
		};

		GLubyte* data{ new GLubyte[buffer_size] };
		
		GLubyte* write_position = data;
		for (GLuint bidirection{ 0u }; bidirection < 2u; ++bidirection)
		{
			GLint min_merge_rays_cascade{ static_cast<GLint>(environment.state.holographic_radiance_cascades.trace_rays_cascade_counts[bidirection]) };
			GLint cascade_count{ static_cast<GLint>(environment.state.holographic_radiance_cascades.cascade_counts[bidirection]) };
			GLint max_cascade{ static_cast<GLint>(cascade_count - 1u) };
			GLint probe_grid_length{ static_cast<GLint>(environment.state.holographic_radiance_cascades.probe_grid_size[bidirection]) };
			GLint orthogonal_bidirection{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_orthogonal_bidirection(bidirection)) };
			GLint orthogonal_probe_grid_length{ static_cast<GLint>(environment.state.holographic_radiance_cascades.probe_grid_size[orthogonal_bidirection]) };
			GLint lower_cascade_max_probe_row{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_probe_row(orthogonal_probe_grid_length)) };

			for (GLuint direction{ 0u }; direction < 2u; ++direction)
			{
				environment.state.holographic_radiance_cascades.configuration.offset_pairs[bidirection][direction].merge_rays = write_position - data;

				for (GLuint cascade{ static_cast<GLuint>(min_merge_rays_cascade) }; cascade < static_cast<GLuint>(cascade_count); ++cascade)
				{
					// TODO: Use uints in shaders.
					GLint cascade_power_of_two{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade)) };
					GLint rays_per_probe{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_rays_per_probe(cascade_power_of_two)) };
					GLint lower_cascade{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_lower_cascade(cascade)) };
					GLint lower_cascade_power_of_two{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(lower_cascade)) };
					GLint lower_cascade_rays_per_probe{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_rays_per_probe(lower_cascade_power_of_two)) };
					GLint lower_cascade_max_probe_column{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_probe_column(probe_grid_length, lower_cascade_power_of_two)) };
					GLint lower_cascade_max_probe_column_texel_x{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_probe_column_texel_x(lower_cascade_max_probe_column, lower_cascade_rays_per_probe)) };

					std::memcpy(write_position + environment.state.layouts.merge_rays_data.rays_per_probe_state.offset, &rays_per_probe, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_rays_data.lower_cascade_rays_per_probe_state.offset, &lower_cascade_rays_per_probe, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_rays_data.lower_cascade_power_of_two_state.offset, &lower_cascade_power_of_two, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_rays_data.lower_cascade_max_probe_column_texel_x_state.offset, &lower_cascade_max_probe_column_texel_x, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_rays_data.lower_cascade_max_probe_row_state.offset, &lower_cascade_max_probe_row, sizeof(GLint));

					write_position += merge_rays_stride;
				}
				
				environment.state.holographic_radiance_cascades.configuration.offset_pairs[bidirection][direction].merge_fluence = write_position - data;
				for (GLuint cascade{ static_cast<GLuint>(max_cascade) }; cascade != minus_1_uint; --cascade)
				{
					GLint cascade_power_of_two{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade)) };
					GLint upper_cascade{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_upper_cascade(cascade)) };
					GLint upper_cascade_power_of_two{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(upper_cascade)) };
					GLint direction_mask{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_direction_mask(cascade_power_of_two)) };
					GLint max_ray_probe_column{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_ray_probe_column(probe_grid_length, cascade_power_of_two)) };
					GLint max_fluence_probe_column_texel_x{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_fluence_probe_column_texel_x(probe_grid_length, upper_cascade_power_of_two, upper_cascade)) };
					GLint max_fluence_probe_y{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_max_fluence_probe_y(orthogonal_probe_grid_length)) };
					GLint rays_per_probe{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_rays_per_probe(cascade_power_of_two)) };
					GLint upper_cascade_probe_column_texel_x_mask{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_upper_cascade_probe_column_texel_x_mask(upper_cascade)) };
					GLint upper_cascade_fluence_layer{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_upper_cascade_fluence_layer(cascade_count, upper_cascade)) };
					GLint output_factor{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_output_factor(cascade, direction)) };
					GLint output_shift{ static_cast<GLint>(::game_logic::holographic_radiance_cascades::compute_output_shift(cascade, direction, probe_grid_length)) };

					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.direction_mask_state.offset, &direction_mask, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.cascade_state.offset, &cascade, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.max_ray_probe_column_state.offset, &max_ray_probe_column, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.max_fluence_probe_column_texel_x_state.offset, &max_fluence_probe_column_texel_x, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.max_fluence_probe_y_state.offset, &max_fluence_probe_y, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.rays_per_probe_state.offset, &rays_per_probe, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.cascade_power_of_two_state.offset, &cascade_power_of_two, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.upper_cascade_probe_column_texel_x_mask_state.offset, &upper_cascade_probe_column_texel_x_mask, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.upper_cascade_state.offset, &upper_cascade, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.upper_cascade_fluence_layer_state.offset, &upper_cascade_fluence_layer, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.output_factor_state.offset, &output_factor, sizeof(GLint));
					std::memcpy(write_position + environment.state.layouts.merge_fluence_data.output_shift_state.offset, &output_shift, sizeof(GLint));

					write_position += merge_fluence_stride;
				}
			}
		}

		glNamedBufferStorage(environment.state.holographic_radiance_cascades.configuration.buffer, buffer_size, data, 0u);

		delete[] data;
	}

	void compile_trace_rays_shaders
	(
		game_environment::Environment& environment,
		::game_state::initialize::compile_shaders::environment::Environment& compile_environment
	)
	{
		GLfloat probe_grid_full_step_to_sample_step_factor[2u]
		{
			::game_logic::holographic_radiance_cascades::compute_probe_grid_full_step_to_sample_step_factor(environment.state.holographic_radiance_cascades.source_width, environment.state.holographic_radiance_cascades.probe_grid_width),
			::game_logic::holographic_radiance_cascades::compute_probe_grid_full_step_to_sample_step_factor(environment.state.holographic_radiance_cascades.source_height, environment.state.holographic_radiance_cascades.probe_grid_height)
		};
		GLuint probe_grid_point_to_sample_point_bias[2u]
		{
			::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_sample_point_bias(environment.state.holographic_radiance_cascades.source_width),
			::game_logic::holographic_radiance_cascades::compute_probe_grid_point_to_sample_point_bias(environment.state.holographic_radiance_cascades.source_height)
		};

		// IMPORTANT TODO: Read these frustum dimensions dynamically in shader.
		GLfloat frustum_unit_z_width{ 2.0f };
		GLfloat frustum_unit_z_height{ 1.0f };
		GLuint probe_grid_full_step_to_sample_step_projection[2u]
		{
			::game_logic::holographic_radiance_cascades::compute_probe_grid_full_step_to_sample_step_projection(frustum_unit_z_width, probe_grid_full_step_to_sample_step_factor[0u]),
			::game_logic::holographic_radiance_cascades::compute_probe_grid_full_step_to_sample_step_projection(frustum_unit_z_height, probe_grid_full_step_to_sample_step_factor[1u])
		};

		for (GLuint bidirection{ 0u }; bidirection < 2u; ++bidirection)
		{
			GLuint trace_rays_shader_count{ environment.state.holographic_radiance_cascades.trace_rays_cascade_counts[bidirection] };
			GLuint base_step_count{ environment.state.holographic_radiance_cascades.trace_rays_base_sample_counts[bidirection] };

			for (GLuint direction{ 0u }; direction < 2u; ++direction)
			{
				GLuint flattened_direction{ ::game_state::holographic_radiance_cascades::directions::flatten[bidirection][direction] };
				environment.state.holographic_radiance_cascades.trace_rays_shaders[bidirection][direction] = new GLuint[trace_rays_shader_count];

				for (GLuint cascade{ 0u }; cascade < trace_rays_shader_count; ++cascade)
				{
					GLuint cascade_power_of_two{ ::game_logic::holographic_radiance_cascades::compute_cascade_power_of_two(cascade) };
					GLuint rays_per_probe{ ::game_logic::holographic_radiance_cascades::compute_rays_per_probe(cascade_power_of_two) };
					GLuint step_count{ base_step_count * cascade_power_of_two };

					GLfloat probe_grid_point_to_sample_point_factor[2u]
					{
						probe_grid_full_step_to_sample_step_factor[0u],
						probe_grid_full_step_to_sample_step_factor[1u],
					};
					probe_grid_point_to_sample_point_factor[bidirection] *= cascade_power_of_two;

					std::string parameter_definitions
					{
						"#define DIRECTION " + std::to_string(flattened_direction) + "\n"

						"const int rays_per_probe = " + std::to_string(static_cast<GLint>(rays_per_probe)) + ";\n"
						"const int cascade_power_of_two = " + std::to_string(static_cast<GLint>(cascade_power_of_two)) + ";\n"
						
						"const vec2 probe_grid_full_step_to_sample_step_factor = vec2(" + std::to_string(probe_grid_full_step_to_sample_step_factor[0u]) + ", " + std::to_string(probe_grid_full_step_to_sample_step_factor[1u]) + ");\n"
						"const vec2 probe_grid_point_to_sample_point_bias = vec2(" + std::to_string(probe_grid_point_to_sample_point_bias[0u]) + ", " + std::to_string(probe_grid_point_to_sample_point_bias[1u]) + ");\n"
						"const vec2 probe_grid_point_to_sample_point_factor = vec2(" + std::to_string(probe_grid_point_to_sample_point_factor[0u]) + ", " + std::to_string(probe_grid_point_to_sample_point_factor[1u]) + ");\n"
						"const vec2 probe_grid_full_step_to_sample_step_projection = vec2(" + std::to_string(probe_grid_full_step_to_sample_step_projection[0u]) + ", " + std::to_string(probe_grid_full_step_to_sample_step_projection[1u]) + ");\n"

						"const int step_count = " + std::to_string(static_cast<GLint>(step_count)) + ";\n"
					};

					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.vertex_shader,
						compile_environment.readonly_prefix_source,
						parameter_definitions,
						::util::shader::file_to_string("draw/holographic_radiance_cascades/trace_rays/trace_rays.vert")
					);

					::util::shader::set_shader_statically
					(
						compile_environment.shader_group.fragment_shader,
						compile_environment.readonly_prefix_source,
						parameter_definitions,
						::util::shader::file_to_string("draw/holographic_radiance_cascades/trace_rays/trace_rays.frag")
					);

					environment.state.holographic_radiance_cascades.trace_rays_shaders[bidirection][direction][cascade] = ::util::shader::create_program
					(
						compile_environment.shader_group.vertex_shader, compile_environment.shader_group.fragment_shader
					);
				}
			}
		}
	}

	void compile_shaders(game_environment::Environment& environment)
	{
		::game_state::initialize::compile_shaders::environment::Environment compile_environment;
		::game_logic::initialize::compile_shaders::environment::initialize(environment, compile_environment);

		compile_trace_rays_shaders(environment, compile_environment);

		::game_logic::initialize::compile_shaders::environment::free(environment, compile_environment);
	}

	void allocate(game_environment::Environment& environment)
	{
		std::cout << "Allocate Holographic Radiance Cascades." << std::endl;

		allocate_source_image(environment);
		allocate_skycircle(environment);
		allocate_rays(environment);
		allocate_angular_fluence(environment);
		allocate_fluence(environment);

		allocate_configuration(environment);

		compile_shaders(environment);
	}
}
