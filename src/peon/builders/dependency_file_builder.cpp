#include "dependency_file_builder.h"

#include "solar/strings/string_build.h"
#include "solar/json/json_array.h"
#include "solar/io/file_system.h"
#include "solar/io/file_path_helpers.h"
#include "solar/io/file_stream_ptr.h"
#include "solar/io/text_reader.h"
#include "solar/utility/alert.h"
#include "solar/utility/trace.h"
#include "peon/peon.h"

namespace solar {

	dependency_file_builder::dependency_file_builder(file_system& file_system) 
		: _file_system(file_system) {
	}

	dependency_file_builder::dependency_file_builder(file_system& file_system, json_object& object) 
		: _file_system(file_system) {

		_src_extension = object.get_string("src_extension");
		
		auto dea = object.get_array("dependent_extensions");
		for (unsigned int i = 0; i < dea.size(); ++i) {
			_dependent_extensions.push_back(dea.get_string(i));
		}

		if (_dependent_extensions.size() == 0) {
			ALERT("no dependent_extensions found");
		}
	}

	std::string dependency_file_builder::to_string() const {
		return build_string("{{ type:DEPENDENCY_FILE , dependent_extensions:{} }}", build_container_string(_dependent_extensions));
	}

	std::unique_ptr<builder> dependency_file_builder::clone(json_object& object) {
		return std::unique_ptr<builder>(new dependency_file_builder(_file_system, object));
	}

	std::string dependency_file_builder::get_src_extension() {
		return _src_extension;
	}

	std::string dependency_file_builder::get_dst_folder() {
		return "";
	}

	checksum dependency_file_builder::get_checksum() const {
		if (_checksum == nullptr) {
			_checksum = std::make_unique<checksum>(checksum()
				.add_string(_src_extension));
		}
		return *_checksum.get();
	}

	bool dependency_file_builder::build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) {
		bool did_build_file = false;

		if (is_forced == build_file_is_forced::YES) {
			did_build_file = build_dependent_files(peon, src_path, "FORCED");
		}
		else {
			auto compare_result = peon.get_memory_registry().compare_to_memory(src_path, get_checksum());
			if (compare_result != memory_compare_result::NO_DIFFERENCES) {
				did_build_file = build_dependent_files(peon, src_path, memory_compare_result_details::get_string(compare_result));
			}
		}

		if (did_build_file) {
			peon.get_memory_registry().register_memory(src_path, get_checksum());
		}

		return did_build_file;
	}

	bool dependency_file_builder::build_dependent_files(peon& peon, const std::string& src_path, const char* reason) {
		auto dependent_file_names = find_dependent_file_names(peon.get_settings().get_src_root(), src_path);
		TRACE("[{}] {} has {} dependent files...", reason, src_path, dependent_file_names.size());
		for (const auto& df : dependent_file_names) {
			peon.try_build_file(df, build_file_is_forced::YES);
		}
		return true;
	}

	std::vector<std::string> dependency_file_builder::find_dependent_file_names(const std::string& find_root, const std::string& src_path) const {
		auto potentially_dependent_file_names = _file_system.find_file_names(
			find_root, 
			find_file_names_options().add_filtered_extensions(_dependent_extensions));
		
		std::vector<std::string> dependent_file_names;
		auto src_file_name = get_file_name_no_path(src_path);
		for (const auto& pdfn : potentially_dependent_file_names) {
			auto fs = make_file_stream_ptr(_file_system, pdfn, file_mode::OPEN_READ);
			if (fs != nullptr) {
				text_reader reader(fs.get());
				std::string line;
				while (reader.read_line(line)) {
					if (line.find(src_file_name) != std::string::npos) {
						dependent_file_names.push_back(pdfn);
						break;
					}
				}
			}
		}
		return dependent_file_names;
	}

}