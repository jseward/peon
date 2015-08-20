#pragma once

#include <string>
#include "solar/json/json_object.h"
#include "solar/io/file_system.h"

namespace solar {

	class peon_settings {
	private:
		std::string _src_root;
		std::string _dst_root;

	public:
		peon_settings();
		void load(file_system& file_system, json_object& object);
		const std::string& get_src_root() const;
		const std::string& get_dst_root() const;
		std::string to_string() const;
	};

}