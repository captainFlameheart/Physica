#pragma once

namespace game_state::layouts::timestamp_metadata
{
	struct timestamp_capacity
	{
		GLint offset;
	};

	struct generation_capacity
	{
		GLint offset;
	};

	struct query_capacity
	{
		GLint offset;
	};

	struct generation_query_capacity
	{
		GLint offset;
	};

	struct generation
	{
		GLint offset;
	};

	struct next_timestamp
	{
		GLint offset;
	};

	struct metadata_color
	{
		GLint offset;
		GLint top_level_array_stride;
	};

	struct metadata_generation
	{
		GLint offset;
	};

	struct metadata_name_base
	{
		GLint offset;
	};

	struct Block
	{
		GLint buffer_data_size;
	};

	struct State
	{
		union
		{
			timestamp_capacity timestamp_capacity_state;
			GLint timestamp_capacity_properties[sizeof(timestamp_capacity) / sizeof(GLint)];
		};
		union
		{
			generation_capacity generation_capacity_state;
			GLint generation_capacity_properties[sizeof(generation_capacity) / sizeof(GLint)];
		};
		union
		{
			query_capacity query_capacity_state;
			GLint query_capacity_properties[sizeof(query_capacity) / sizeof(GLint)];
		};
		union
		{
			generation_query_capacity generation_query_capacity_state;
			GLint generation_query_capacity_properties[sizeof(generation_query_capacity) / sizeof(GLint)];
		};

		union
		{
			generation generation_state;
			GLint generation_properties[sizeof(generation) / sizeof(GLint)];
		};
		union
		{
			next_timestamp next_timestamp_state;
			GLint next_timestamp_properties[sizeof(next_timestamp) / sizeof(GLint)];
		};

		union
		{
			metadata_color metadata_color_state;
			GLint metadata_color_properties[sizeof(metadata_color) / sizeof(GLint)];
		};
		union
		{
			metadata_generation metadata_generation_state;
			GLint metadata_generation_properties[sizeof(metadata_generation) / sizeof(GLint)];
		};
		union
		{
			metadata_name_base metadata_name_base_state;
			GLint metadata_name_base_properties[sizeof(metadata_name_base) / sizeof(GLint)];
		};

		union
		{
			Block block_state;
			GLint block_properties[sizeof(Block) / sizeof(GLint)];
		};
	};
}
