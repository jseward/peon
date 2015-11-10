#pragma once

#include "builder.h"

namespace solar {

	class file_system;

	//this builder handles files that contain configuration options for building side-by-side files.
	//
	//ex. button.tga has dds compression configuration in button.peon.

	class build_config_file_builder : public builder {
	private:
		file_system& _file_system;

		std::string _src_extension;
		std::string _config_target_extension;

		mutable std::unique_ptr<checksum> _checksum;

	public:
		build_config_file_builder(file_system& file_system);
		build_config_file_builder(file_system& file_system, json_object& object);
		virtual std::string to_string() const override;
		virtual std::unique_ptr<builder> clone(json_object& object) override;
		virtual std::string get_src_extension() override;
		virtual std::string get_dst_folder() override;
		virtual bool build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) override;

	private:
		checksum get_checksum() const;
		bool build_config_target_file(peon& peon, const std::string& src_path, const char* reason);
	};

}