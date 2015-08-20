#include "peon_settings.h"
#include "solar/utility/trace.h"
#include <iomanip>

namespace solar {

	peon_settings::peon_settings() {
	}

	void peon_settings::load(file_system& file_system, json_object& object) {
		auto raw_src_root = object.get_string("src_root");
		auto raw_dst_root = object.get_string("dst_root");

		_src_root = file_system.resolve_file_path(raw_src_root);
		_dst_root = file_system.resolve_file_path(raw_dst_root);
	}

	const std::string& peon_settings::get_src_root() const {
		return _src_root;
	}

	const std::string& peon_settings::get_dst_root() const {
		return _dst_root;
	}

	std::string peon_settings::to_string() const {
		std::ostringstream out;
		out << std::left << std::setw(20) << "src_root" << std::setw(0) << "'" << _src_root << "'" << "\n";
		out << std::left << std::setw(20) << "dst_root" << std::setw(0) << "'" << _dst_root << "'" << "\n";
		return out.str();
	}

}