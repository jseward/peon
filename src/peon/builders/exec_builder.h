#pragma once

#include "builder.h"

namespace solar {

	class exe_runner;
	class file_system;

	//this builder will run a command line executable to build

	class exec_builder : public builder {
	private:
		file_system& _file_system;
		exe_runner& _exe_runner;

		std::string _src_extension;
		std::string _dst_folder;
		std::string _dst_extension;
		std::string _exe_path;
		std::string _exe_arguments_format;

		mutable std::unique_ptr<checksum> _checksum;

	public:
		exec_builder(file_system& file_system, exe_runner& exe_runner);
		exec_builder(file_system& file_system, exe_runner& exe_runner, json_object& object);
		virtual std::string to_string() const override;
		virtual std::unique_ptr<builder> clone(json_object& object) override;
		virtual std::string get_src_extension() override;
		virtual std::string get_dst_folder() override;
		virtual bool build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) override;

	private:
		checksum get_checksum() const;
		bool run_exe(const std::string& src_path, const std::string& dst_path, const char* reason);
	};

}
