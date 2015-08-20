// peon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "solar/engines/win32_cli_engine.h"
#include "solar/utility/trace.h"
#include "solar/utility/command_line_parser.h"
#include "solar/io/file_path_helpers.h"
#include "peon/peon.h"

using namespace solar;

struct app {
public:
	win32_cli_engine _engine;
	peon _peon;

public:
	app()
		: _peon(
			_engine._win32_file_system,
			_engine._win32_directory_change_watcher,
			_engine._win32_exe_runner) {
	}
};

app g_app; //global so can watch in debugger to get everything easily.

int _tmain(int argc, _TCHAR* argv[])
{
	g_app._engine.setup(win32_cli_engine_setup_params()
		.set_app_params(win32_cli_app_setup_params()
			.set_alert_behavoir(win32_cli_app_error_behavoir::WRITE_AND_CONTINUE)
			.set_assert_behavoir(win32_cli_app_error_behavoir::WRITE_AND_CONTINUE)));
	g_app._peon.setup();
	
	auto parser = command_line_parser()
		.add_switch('w', "watch", "watches source directory for any changes")
		.add_optional_value('i', "input", "attempts to build one single input file", "")
		.add_switch('f', "force", "forces the build to be done (ignore memory of previous builds)");

	if (parser.execute(argc, argv)) {
		auto is_forced = parser.is_switch_set("force") ?
			build_file_is_forced::YES :
			build_file_is_forced::NO;

		if (parser.is_switch_set("watch")) {
			g_app._peon.build_all(build_file_is_forced::NO); //build all first to pick up changes while peon was not running!
			g_app._peon.begin_watching();
			TRACE("press any key to exit...", g_app._peon.get_settings().get_src_root());
			g_app._engine.wait_for_user_input();
			g_app._peon.end_watching();
		}
		else if (parser.is_value_set("input")) {
			auto file_name = parser.get_value("input");
			if (g_app._engine._win32_file_system.does_file_exist(file_name)) {
				g_app._peon.try_build_file(file_name, is_forced);
			}
			else {
				auto full_file_name = make_file_path(g_app._peon.get_settings().get_src_root(), file_name);
				g_app._peon.try_build_file(full_file_name, is_forced);
			}
		}
		else {
			g_app._peon.build_all(is_forced);
		}
	}

	g_app._peon.teardown();
	g_app._engine.teardown();
	
	return 0;
}

