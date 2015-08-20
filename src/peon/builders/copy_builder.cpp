#include "copy_builder.h"

#include "solar/strings/string_build.h"
#include "solar/io/file_path_helpers.h"
#include "solar/io/file_system.h"
#include "solar/utility/trace.h"
#include "peon/peon.h"

namespace solar {

	copy_builder::copy_builder(file_system& file_system) 
		: _file_system(file_system) {
	}

	copy_builder::copy_builder(file_system& file_system, json_object& object) 
		: _file_system(file_system) {
		_src_extension = object.get_string("src_extension");
		_dst_folder = object.get_string("dst_folder");
	}

	std::string copy_builder::to_string() const {
		return build_string("{{ type:COPY }}");
	}

	checksum copy_builder::get_checksum() const {
		if (_checksum == nullptr) {
			_checksum = std::make_unique<checksum>(checksum()
				.add_string(_src_extension)
				.add_string(_dst_folder));
		}
		return *_checksum.get();
	}

	std::unique_ptr<builder> copy_builder::clone(json_object& object) {
		return std::unique_ptr<builder>(new copy_builder(_file_system, object));
	}

	std::string copy_builder::get_src_extension() {
		return _src_extension;
	}

	std::string copy_builder::get_dst_folder() {
		return _dst_folder;
	}

	bool copy_builder::build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) {
		bool did_build_file = false;

		auto src_file_name = get_file_name_no_path(src_path);
		auto dst_folder_path = peon.make_dst_folder_path(_dst_folder);
		auto dst_path = make_file_path(dst_folder_path, src_file_name);

		if (is_forced == build_file_is_forced::YES) {
			did_build_file = copy_file(src_path, dst_path, "FORCED");
		}
		else {
			auto compare_result = peon.get_memory_registry().compare_to_memory(src_path, dst_path, get_checksum());
			if (compare_result != memory_compare_result::NO_DIFFERENCES) {
				did_build_file = copy_file(src_path, dst_path, memory_compare_result_details::get_string(compare_result));
			}
		}

		if (did_build_file) {
			peon.get_memory_registry().register_memory(src_path, dst_path, get_checksum());
		}

		return did_build_file;
	}

	bool copy_builder::copy_file(const std::string& src_path, const std::string& dst_path, const char* reason) {
		TRACE("[{}] {} -> {}", reason, src_path, dst_path);
		if (_file_system.copy_file(src_path, dst_path)) {
			_file_system.set_is_file_read_only(dst_path, false);
			return true;
		}
		return false;
	}

}