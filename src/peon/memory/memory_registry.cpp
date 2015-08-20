#include "memory_registry.h"
#include "solar/utility/assert.h"
#include "solar/utility/alert.h"
#include "solar/utility/verify.h"
#include "solar/utility/trace.h"
#include "solar/io/file_system.h"
#include "solar/io/file_stream_ptr.h"
#include "solar/json/json_document.h"
#include "solar/json/json_object_builder.h"
#include "solar/json/json_array_builder.h"

namespace solar {

	memory_registry::memory_registry(file_system& file_system)
		: _file_system(file_system) {
	}

	void memory_registry::load(const std::string& path) {
		TRACE("loading memory_registry from '{}'", path);

		_memory_map.clear();

		if (!_file_system.does_file_exist(path)) {
			TRACE("loading memory_registry skipped (file does not exist)");
		}
		else {
			auto fs = make_file_stream_ptr(_file_system, path, file_mode::OPEN_READ);
			if (fs != nullptr) {
				auto doc = json_document(*fs);
				auto arr = convert_json_document_to_json_array(doc);
				for (unsigned int i = 0; i < arr.size(); ++i) {
					auto mem = make_memory_from_json(arr.get_object(i));
					if (_memory_map.find(mem._src_path_key) != _memory_map.end()) {
						ALERT("duplicate src_path_key found : '{}'", mem._src_path_key);
					}
					else {
						_memory_map[mem._src_path_key] = mem;
					}
				}
			}
		}
	}

	void memory_registry::save(const std::string& path) {
		using Allocator = rapidjson::MemoryPoolAllocator<>;
		Allocator allocator;

		json_array_builder<Allocator> array_builder(allocator);
		array_builder.reserve(_memory_map.size());

		for (auto memory : _memory_map) {
			json_object_builder<Allocator> memory_object_builder(allocator);
			memory.second.build_json_object(memory_object_builder);
			array_builder.push_back_object(memory_object_builder);
		}

		auto fs = make_file_stream_ptr(_file_system, path, file_mode::CREATE_WRITE);
		if (fs != nullptr) {
			array_builder.write_to_stream(*fs);
		}
	}

	void memory_registry::register_memory(const std::string& src_path, const checksum& builder_checksum) {
		register_memory(memory(
			src_path,
			make_file_state(src_path),
			builder_checksum));
	}

	void memory_registry::register_memory(const std::string& src_path, const std::string& dst_path, const checksum& builder_checksum) {
		register_memory(memory(
			src_path,
			make_file_state(src_path),
			make_file_state(dst_path),
			builder_checksum));
	}

	void memory_registry::register_memory(const memory& m) {
		auto iter = _memory_map.find(m._src_path_key);
		if (iter == _memory_map.end()) {
			_memory_map[m._src_path_key] = m;
		}
		else {
			iter->second = m;
		}
	}

	memory_compare_result memory_registry::compare_to_memory(const std::string& src_path, const checksum& builder_checksum) const {
		return compare_to_memory(src_path, std::vector<std::string>({ src_path }), std::vector<std::string>(), builder_checksum);
	}

	memory_compare_result memory_registry::compare_to_memory(const std::string& src_path, const std::string& dst_path, const checksum& builder_checksum) const {
		return compare_to_memory(src_path, std::vector<std::string>({ src_path }), std::vector<std::string>({ dst_path }), builder_checksum);
	}

	memory_compare_result memory_registry::compare_to_memory(const std::string& src_path_key, const std::vector<std::string>& src_paths, const std::vector<std::string>& dst_paths, const checksum& builder_checksum) const {
		auto iter = _memory_map.find(src_path_key);
		if (iter == _memory_map.end()) {
			return memory_compare_result::NO_MEMORY_FOUND;
		}

		if (iter->second._builder_checksum != builder_checksum) {
			return memory_compare_result::BUILDER_CHECKSUM_CHANGED;
		}

		if (iter->second._src_file_states.size() != src_paths.size()) {
			return memory_compare_result::SRC_FILE_COUNT_CHANGED;
		}

		for (unsigned int src_i = 0; src_i < src_paths.size(); ++src_i) {
			if (iter->second._src_file_states.at(src_i) != make_file_state(src_paths.at(src_i))) {
				return memory_compare_result::SRC_FILE_CHANGED;
			}
		}

		if (iter->second._dst_file_states.size() != dst_paths.size()) {
			return memory_compare_result::DST_FILE_COUNT_CHANGED;
		}

		for (unsigned int dst_i = 0; dst_i < dst_paths.size(); ++dst_i) {
			if (!_file_system.does_file_exist(dst_paths.at(dst_i))) {
				return memory_compare_result::DST_FILE_NOT_FOUND;
			}

			if (iter->second._dst_file_states.at(dst_i) != make_file_state(dst_paths.at(dst_i))) {
				return memory_compare_result::DST_FILE_CHANGED;
			}
		}

		return memory_compare_result::NO_DIFFERENCES;
	}

	memory::file_state memory_registry::make_file_state(const std::string& path) const {
		date_time last_write_time;
		VERIFY(_file_system.try_get_last_write_time(last_write_time, path));
		return memory::file_state(path, last_write_time);
	}

}
