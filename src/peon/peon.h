#pragma once

#include <vector>
#include <memory>
#include "peon_settings.h"
#include "peon_watcher.h"
#include "peon/builders/builder_factory.h"
#include "peon/memory/memory_registry.h"

namespace solar {

	class file_system;
	class directory_change_watcher;
	class exe_runner;

	class peon {
	private:
		file_system& _file_system;

		peon_settings _settings;
		std::unique_ptr<memory_registry> _memory_registry;
		std::unique_ptr<builder_factory> _builder_factory;
		std::vector<std::shared_ptr<builder>> _builders;
		std::unordered_map<std::string, std::shared_ptr<builder>> _extension_builder_map;
		peon_watcher _watcher;

	public:
		peon(
			file_system& file_system,
			directory_change_watcher& directory_change_watcher,
			exe_runner& exe_runner);

		void setup();
		void teardown();

		const peon_settings& get_settings() const;
		memory_registry& get_memory_registry();

		void build_all(build_file_is_forced is_forced);
		bool try_build_file(const std::string& path, build_file_is_forced is_forced);

		void begin_watching();
		void end_watching();

		std::string make_dst_folder_path(const std::string& dst_folder);

	private:
		void load_settings();
		void load_builders();
		void print_settings();
		void print_builders();
		void create_builder_dst_folders();
		builder* get_builder_for_file(const std::string& file_name);
		std::string make_memory_registry_path();
	};

}