#pragma once

#include "builder.h"

namespace solar {

	class file_system;

	//this builder handles files that are dependencies of other data files. it isn't built itself into anything but
	//causes other data files to be built if it changes (as they are dependent on it).
	//
	//ex. .fxh files that .fx files #include

	class dependency_file_builder : public builder {
	private:
		file_system& _file_system;

		std::string _src_extension;
		std::vector<std::string> _dependent_extensions;

		mutable std::unique_ptr<checksum> _checksum;

	public:
		dependency_file_builder(file_system& file_system);
		dependency_file_builder(file_system& file_system, json_object& object);
		virtual std::string to_string() const override;
		virtual std::unique_ptr<builder> clone(json_object& object) override;
		virtual std::string get_src_extension() override;
		virtual std::string get_dst_folder() override;
		virtual bool build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) override;

	private:
		checksum get_checksum() const;
		bool build_dependent_files(peon& peon, const std::string& src_path, const char* reason);
		std::vector<std::string> find_dependent_file_names(const std::string& find_root, const std::string& src_path) const;
	};

}