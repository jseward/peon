#pragma once

#include <string>
#include <unordered_map>
#include "memory.h"
#include "memory_compare_result.h"

namespace solar {

	class file_system;

	//needs to handle
	//- one src file -> no dst files (ex. dependency .h files)
	//- one src file -> one dst file (most common)
	//- one src file -> many dst files
	//- many src files -> one dst file (ex. zipping folder)

	class memory_registry {

	private:
		file_system& _file_system;

		std::unordered_map<std::string, memory> _memory_map;

	public:
		memory_registry(file_system& file_system);

		void load(const std::string& path);
		void save(const std::string& path);

		void register_memory(const std::string& src_path, const checksum& builder_checksum);
		void register_memory(const std::string& src_path, const std::string& dst_path, const checksum& builder_checksum);

		memory_compare_result compare_to_memory(const std::string& src_path, const checksum& builder_checksum) const;
		memory_compare_result compare_to_memory(const std::string& src_path, const std::string& dst_path, const checksum& builder_checksum) const;
		memory_compare_result compare_to_memory(const std::string& src_path_key, const std::vector<std::string>& src_paths, const std::vector<std::string>& dst_paths, const checksum& builder_checksum) const;

	private:
		memory::file_state make_file_state(const std::string& path) const;
		void register_memory(const memory& m);
	};



}