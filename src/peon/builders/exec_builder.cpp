#include "exec_builder.h"

#include "solar/strings/string_build.h"
#include "solar/io/file_path_helpers.h"
#include "solar/io/file_system.h"
#include "solar/utility/exe_runner.h"
#include "solar/utility/alert.h"
#include "solar/utility/verify.h"
#include "solar/utility/trace.h"
#include "peon/peon.h"

namespace solar {

	exec_builder::exec_builder(file_system& file_system, exe_runner& exe_runner)
		: _file_system(file_system)
		, _exe_runner(exe_runner) {
	}

	exec_builder::exec_builder(file_system& file_system, exe_runner& exe_runner, json_object& object)
		: _file_system(file_system)
		, _exe_runner(exe_runner) {

		_src_extension = object.get_string("src_extension");
		_dst_folder = object.get_string("dst_folder");
		_dst_extension = object.get_string("dst_extension");
		_exe_path = object.get_string("exe_path");
		_exe_arguments_format = object.get_string("exe_arguments_format");
	}

	std::string exec_builder::to_string() const {
		return build_string("{{ type:EXEC , exe_path:'{}' , exe_arguments_format:'{}' }}", _exe_path, _exe_arguments_format);
	}

	checksum exec_builder::get_checksum() const {
		if (_checksum == nullptr) {
			date_time exe_last_write_time;
			_file_system.try_get_last_write_time(exe_last_write_time, _exe_path); //ignoring failures (ex. exe doesn't exist), would cause checksum to change

			_checksum = std::make_unique<checksum>(checksum()
				.add_string(_src_extension)
				.add_string(_dst_folder)
				.add_string(_dst_extension)
				.add_string(_exe_path)
				.add_string(_exe_arguments_format)
				.add_date_time(exe_last_write_time));
		}
		return *_checksum.get();
	}

	std::unique_ptr<builder> exec_builder::clone(json_object& object) {
		return std::unique_ptr<builder>(new exec_builder(_file_system, _exe_runner, object));
	}

	std::string exec_builder::get_src_extension() {
		return _src_extension;
	}

	std::string exec_builder::get_dst_folder() {
		return _dst_folder;
	}

	bool exec_builder::build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) {
		bool did_build_file = false;

		auto src_file_name = get_file_name_no_path(src_path);
		auto dst_folder_path = peon.make_dst_folder_path(_dst_folder);
		auto dst_path = change_file_path_extension(make_file_path(dst_folder_path, src_file_name), _dst_extension);

		if (is_forced == build_file_is_forced::YES) {
			did_build_file = run_exe(src_path, dst_path, "FORCED");
		}
		else {
			auto compare_result = peon.get_memory_registry().compare_to_memory(src_path, dst_path, get_checksum());
			if (compare_result != memory_compare_result::NO_DIFFERENCES) {
				did_build_file = run_exe(src_path, dst_path, solar::to_string(compare_result));
			}
		}

		if (did_build_file) {
			peon.get_memory_registry().register_memory(src_path, dst_path, get_checksum());
		}

		return did_build_file;
	}

	bool exec_builder::run_exe(const std::string& src_path, const std::string& dst_path, const char* reason) {
		auto exe_args = build_string(_exe_arguments_format.c_str(), src_path, dst_path);
		TRACE("[{}] {} {}", reason, _exe_path, exe_args);

		auto result = _exe_runner.run_exe(_exe_path, exe_args);
		if (!result._success) {
			ALERT("failed to build file. running exe failed.\nexe_path: '{}'\nexe_args: '{}'\nerror: {}", _exe_path, exe_args, result._error_message);
			return false;
		}

		if (result._exit_code != 0) {
			ALERT("failed to build file. exit_code != 0.\nexe_path: '{}'\nexe_args: '{}'\nexit_code: {}\nstd_out:\n{}\nstd_err:\n{}\n", _exe_path, exe_args, result._exit_code, result._std_out, result._std_err);
			return false;
		}

		if (!result._std_err.empty()) {
			ALERT("failed to build file. std_err is not empty.\nexe_path: '{}'\nexe_args: '{}'\nstd_out:\n{}\nstd_err:\n{}\n", _exe_path, exe_args, result._std_out, result._std_err);
			return false;
		}

		return true;
	}

}
