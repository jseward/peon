#include "build_config_file_builder.h"

#include "solar/strings/string_build.h"
#include "solar/io/file_path_helpers.h"
#include "peon/peon.h"

namespace solar {

	build_config_file_builder::build_config_file_builder(file_system& file_system)
		: _file_system(file_system) {
	}

	build_config_file_builder::build_config_file_builder(file_system& file_system, json_object& object)
		: _file_system(file_system) {

		_src_extension = object.get_string("src_extension");
		_config_target_extension = object.get_string("config_target_extension");
	}

	std::string build_config_file_builder::to_string() const {
		return build_string("{{ type:BUILD_CONFIG_FILE , config_target_ext:{} }}", _config_target_extension);
	}

	std::unique_ptr<builder> build_config_file_builder::clone(json_object& object) {
		return std::unique_ptr<builder>(new build_config_file_builder(_file_system, object));
	}

	std::string build_config_file_builder::get_src_extension() {
		return _src_extension;
	}

	std::string build_config_file_builder::get_dst_folder() {
		return "";
	}

	checksum build_config_file_builder::get_checksum() const {
		if (_checksum == nullptr) {
			_checksum = std::make_unique<checksum>(checksum()
				.add_string(_src_extension)
				.add_string(_config_target_extension));
		}
		return *_checksum.get();
	}

	bool build_config_file_builder::build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) {
		bool did_build_file = false;

		if (is_forced == build_file_is_forced::YES) {
			did_build_file = build_config_target_file(peon, src_path, "FORCED");
		}
		else {
			auto compare_result = peon.get_memory_registry().compare_to_memory(src_path, get_checksum());
			if (compare_result != memory_compare_result::NO_DIFFERENCES) {
				did_build_file = build_config_target_file(peon, src_path, solar::to_string(compare_result));
			}
		}

		if (did_build_file) {
			peon.get_memory_registry().register_memory(src_path, get_checksum());
		}

		return did_build_file;
	}

	bool build_config_file_builder::build_config_target_file(peon& peon, const std::string& src_path, const char* reason) {
		auto config_target_path = change_file_path_extension(src_path, _config_target_extension);
		return peon.try_build_file(config_target_path, build_file_is_forced::YES);
	}

}