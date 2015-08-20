#include "memory.h"
#include "solar/json/json_object.h"

namespace solar {

	memory::file_state::file_state() {
	}

	memory::file_state::file_state(const std::string& path, date_time last_write_time) 
		: _path(path)
		, _last_write_time(last_write_time) {
	}

	bool memory::file_state::operator==(const file_state& rhs) const {
		return
			_path == rhs._path &&
			_last_write_time == rhs._last_write_time;
	}

	bool memory::file_state::operator!=(const file_state& rhs) const {
		return !(*this == rhs);
	}

	memory::memory() {
	}

	memory::memory(
		const std::string& src_path_key,
		const file_state& src_file_state,
		const checksum& builder_checksum)
		: _src_path_key(src_path_key)
		, _builder_checksum(builder_checksum) {

		_src_file_states.push_back(src_file_state);
	}

	memory::memory(
		const std::string& src_path_key,
		const file_state& src_file_state,
		const file_state& dst_file_state,
		const checksum& builder_checksum)
		: _src_path_key(src_path_key)
		, _builder_checksum(builder_checksum) {

		_src_file_states.push_back(src_file_state);
		_dst_file_states.push_back(dst_file_state);
	}

	memory::memory(
		const std::string& src_path_key,
		const std::vector<file_state>& src_file_states,
		const std::vector<file_state>& dst_file_states,
		const checksum& builder_checksum)
		: _src_path_key(src_path_key)
		, _src_file_states(src_file_states)
		, _dst_file_states(dst_file_states)
		, _builder_checksum(builder_checksum) {
	}

	memory make_memory_from_json(json_object obj) {
		obj.set_should_track_used_values();
		memory m;
		m._src_path_key = obj.get_string("src_path_key");
		m._src_file_states = make_file_states_from_json(obj.get_array("src_file_states"));
		m._dst_file_states = make_file_states_from_json(obj.get_array("dst_file_states"));
		m._builder_checksum = checksum(obj.get_int("builder_checksum"));
		obj.raise_error_for_unused_values("memory");
		return m;
	}

	std::vector<memory::file_state> make_file_states_from_json(json_array arr) {
		std::vector<memory::file_state> file_states;
		file_states.reserve(arr.size());
		for (unsigned int i = 0; i < arr.size(); ++i) {
			file_states.push_back(make_file_state_from_json(arr.get_object(i)));
		}
		return file_states;
	}

	memory::file_state make_file_state_from_json(json_object obj) {
		obj.set_should_track_used_values();
		memory::file_state fs;
		fs._path = obj.get_string("path");
		fs._last_write_time = obj.get_date_time("last_write_time");
		obj.raise_error_for_unused_values("file_state");
		return fs;
	}

}