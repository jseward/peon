#include "peon.h"
#include <iomanip>
#include "solar/io/file_system.h"
#include "solar/io/file_stream_ptr.h"
#include "solar/io/file_path_helpers.h"
#include "solar/io/directory_change_watcher.h"
#include "solar/json/json_document.h"
#include "solar/utility/trace.h"
#include "solar/utility/alert.h"
#include "solar/utility/container_helpers.h"

namespace solar {

	peon::peon(file_system& file_system, directory_change_watcher& directory_change_watcher, exe_runner& exe_runner)
		: _file_system(file_system) 
		, _watcher(*this, file_system, directory_change_watcher) {

		_memory_registry = std::make_unique<memory_registry>(file_system);
		_builder_factory = std::make_unique<builder_factory>(file_system, exe_runner);
	}

	void peon::setup() {
		load_settings();
		load_builders();
		_memory_registry->load(make_memory_registry_path());
		print_settings();
		print_builders();
		create_builder_dst_folders();
	}

	void peon::teardown() {
		_memory_registry->save(make_memory_registry_path());
	}

	void peon::build_all(build_file_is_forced is_forced) {
		TRACE("building all files ...");
		auto src_root = _file_system.resolve_file_path(_settings.get_src_root());
		auto src_file_names = _file_system.find_file_names(src_root, find_file_names_options().set_is_recursive(true));
		TRACE("found {} files in {}", src_file_names.size(), src_root);
		int build_count = 0;
		for (auto& src_file_path : src_file_names) {
			if (try_build_file(src_file_path, is_forced)) {
				build_count++;
			}
		}
		TRACE("built {} files , ignored {} files", build_count, src_file_names.size() - build_count);
	}

	bool peon::try_build_file(const std::string& path, build_file_is_forced is_forced) {
		bool was_built = false;

		auto builder = get_builder_for_file(path);
		if (builder == nullptr) {
			ALERT("No builder found for '{}'", path);
		}
		else {
			if (builder->build_file(*this, path, is_forced)) {
				was_built = true;
			}
		}

		return was_built;
	}

	void peon::begin_watching() {
		_watcher.begin_watching();
	}

	void peon::end_watching() {
		_watcher.end_watching();
	}

	std::string peon::make_dst_folder_path(const std::string& dst_folder) {
		auto dst_root = _file_system.resolve_file_path(_settings.get_dst_root());
		return make_file_path(dst_root, dst_folder);
	}

	builder* peon::get_builder_for_file(const std::string& file_name) {
		auto extension = get_file_extension(file_name);
		auto iter = _extension_builder_map.find(extension);
		if (iter != _extension_builder_map.end()) {
			return iter->second.get();
		}
		return nullptr;
	}

	const peon_settings& peon::get_settings() const {
		return _settings;
	}

	memory_registry& peon::get_memory_registry() {
		return *_memory_registry.get();
	}

	void peon::load_settings() {
		auto settings_path = _file_system.resolve_file_path("settings.json");
		TRACE("loading settings from '{}'", settings_path);
		auto fs = make_file_stream_ptr(_file_system, settings_path, file_mode::OPEN_READ);
		if (fs != nullptr) {
			auto doc = json_document(*fs);
			_settings.load(_file_system, convert_json_document_to_json_object(doc));
		}
	}

	void peon::load_builders() {
		auto builders_path = _file_system.resolve_file_path("builders.json");
		TRACE("loading builders from '{}'", builders_path);
		auto fs = make_file_stream_ptr(_file_system, builders_path, file_mode::OPEN_READ);
		if (fs != nullptr) {
			auto doc = json_document(*fs);
			auto arr = convert_json_document_to_json_array(doc);
			for (unsigned int i = 0; i < arr.size(); ++i) {
				auto b = _builder_factory->create_builder(arr.get_object(i));
				if (b != nullptr) {
					auto shared_b = std::shared_ptr<builder>(std::move(b));
					
					_builders.push_back(shared_b);
					
					auto ext = shared_b->get_src_extension();
					if (_extension_builder_map.find(ext) != _extension_builder_map.end()) {
						ALERT("file extension mapped to multiple buliders\nbuilder 0 : {}\nbuilder 1 : {}", _extension_builder_map[ext]->to_string(), shared_b->to_string());
					}
					else {
						_extension_builder_map[ext] = shared_b;
					}
				}
			}
		}
	}

	void peon::print_settings() {
		TRACE("\n-- settings --\n{}", _settings.to_string());
	}

	void peon::print_builders() {
		TRACE("\n-- builders --");
		for (auto eb : _extension_builder_map) {
			std::ostringstream os;
			os << std::left << std::setw(30) << eb.first << eb.second->to_string();
			TRACE("{}", os.str());
		}
		TRACE("\n");
	}

	std::string peon::make_memory_registry_path() {
		return _file_system.make_user_file_path(".user", "build_memory.build_memory");
	}

	void peon::create_builder_dst_folders() {
		if (!_file_system.does_directory_exist(_settings.get_dst_root())) {
			TRACE("dst_root does not exist. creating...");
			_file_system.create_directory(_settings.get_dst_root());
		}

		for (auto& builder : _builders) {
			auto dst_folder_path = make_file_path(_settings.get_dst_root(), builder->get_dst_folder());
			if (!_file_system.does_directory_exist(dst_folder_path)) {
				TRACE("dst_folder does not exist. creating...");
				_file_system.create_directory(dst_folder_path);
			}
		}
	}

}