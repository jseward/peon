#pragma once

#include <string>
#include <vector>
#include "solar/time/date_time.h"
#include "solar/json/json_object.h"
#include "solar/json/json_array.h"
#include "solar/json/json_object_builder.h"
#include "solar/json/json_array_builder.h"
#include "solar/utility/checksum.h"

namespace solar {

	class json_object;

	class memory {
	public:
		class file_state {
		public:
			std::string _path;
			date_time _last_write_time;

		public:
			file_state();
			file_state(const std::string& path, date_time last_write_time);
			file_state(json_object obj);

			bool operator==(const file_state& rhs) const;
			bool operator!=(const file_state& rhs) const;

			template<typename AllocatorT> void build_json_object(
				json_object_builder<AllocatorT>& builder) const;

			friend file_state make_file_state_from_json(json_object obj);
		};

	public:
		std::string _src_path_key;
		std::vector<file_state> _src_file_states;
		std::vector<file_state> _dst_file_states;
		checksum _builder_checksum;

	public:
		memory();
		memory(
			const std::string& src_path_key,
			const file_state& src_file_state,
			const checksum& builder_checksum);
		memory(
			const std::string& src_path_key,
			const file_state& src_file_state,
			const file_state& dst_file_state,
			const checksum& builder_checksum);
		memory(
			const std::string& src_path_key,
			const std::vector<file_state>& src_file_states,
			const std::vector<file_state>& dst_file_states,
			const checksum& builder_checksum);

		template<typename AllocatorT> void build_json_object(
			json_object_builder<AllocatorT>& builder) const;

	private:
		template<typename AllocatorT> void build_file_states_json_array(
			json_array_builder<AllocatorT>& array_builder,
			const std::vector<file_state>& file_states) const;

	public:
		friend memory make_memory_from_json(json_object obj);
		friend std::vector<memory::file_state> make_file_states_from_json(json_array arr);
	};

	template<typename AllocatorT> 
	void memory::build_json_object(json_object_builder<AllocatorT>& builder) const {
		json_array_builder<AllocatorT> src_file_states_array(builder.get_allocator());
		build_file_states_json_array(src_file_states_array, _src_file_states);

		json_array_builder<AllocatorT> dst_file_states_array(builder.get_allocator());
		build_file_states_json_array(dst_file_states_array, _dst_file_states);

		builder
			.add_string("src_path_key", _src_path_key)
			.add_array("src_file_states", src_file_states_array)
			.add_array("dst_file_states", dst_file_states_array)
			.add_int("builder_checksum", _builder_checksum.to_int());
	}

	template<typename AllocatorT> void memory::build_file_states_json_array(
		json_array_builder<AllocatorT>& array_builder, 
		const std::vector<file_state>& file_states) const {

		for (const auto& fs : file_states) {
			json_object_builder<AllocatorT> fs_object(array_builder.get_allocator());
			fs.build_json_object(fs_object);
			array_builder.push_back_object(fs_object);
		}
	}

	template<typename AllocatorT>
	void memory::file_state::build_json_object(json_object_builder<AllocatorT>& builder) const {
		builder.add_string("path", _path);
		builder.add_date_time("last_write_time", _last_write_time);
	}

}