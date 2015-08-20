#pragma once

#include <memory>
#include <thread>

namespace solar {

	class peon;
	class file_system;
	class directory_change_watcher;

	class peon_watcher {
	private:
		peon& _peon;
		file_system& _file_system;
		directory_change_watcher& _directory_change_watcher;

		//have to process changes in a seperate thread so the main thread can wait on user input (key press to exit)
		std::unique_ptr<std::thread> _change_handler_thread;
		bool _should_exit_change_handler_thread;

	public:
		peon_watcher(peon& peon, file_system& file_system, directory_change_watcher& directory_change_watcher);
		~peon_watcher();

		void begin_watching();
		void end_watching();

	private:
		void handle_changes_in_thread();
	};

}