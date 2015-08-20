#include "peon_watcher.h"
#include "peon.h"
#include "solar/utility/trace.h"
#include "solar/io/directory_change_watcher.h"
#include "solar/io/file_system.h"

namespace solar {

	peon_watcher::peon_watcher(peon& peon, file_system& file_system, directory_change_watcher& directory_change_watcher) 
		: _peon(peon)
		, _file_system(file_system)
		, _directory_change_watcher(directory_change_watcher)
		, _should_exit_change_handler_thread(false) {
	}

	peon_watcher::~peon_watcher() {
		end_watching();
	}

	void peon_watcher::begin_watching() {
		_directory_change_watcher.begin_watching_directory(_peon.get_settings().get_src_root(), true);

		_should_exit_change_handler_thread = false;
		_change_handler_thread = std::make_unique<std::thread>(&peon_watcher::handle_changes_in_thread, this);
	}

	void peon_watcher::end_watching() {
		_directory_change_watcher.end_watching_directory();

		if (_change_handler_thread != nullptr) {
			_should_exit_change_handler_thread = true;
			_change_handler_thread->join();
			_change_handler_thread.release();
		}
	}

	void peon_watcher::handle_changes_in_thread() {
		while (!_should_exit_change_handler_thread) {

			directory_change change;
			if (_directory_change_watcher.try_pop_directory_change(change)) {
				if (change.get_change_type() != directory_change_type::ITEM_REMOVED) {
					if (_file_system.is_directory(change.get_path())) {
						//ignore directories
					}
					else {
						TRACE("file change detected ({}) : {}", change.get_change_type(), change.get_path());
						_peon.try_build_file(change.get_path(), build_file_is_forced::NO);
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

}
