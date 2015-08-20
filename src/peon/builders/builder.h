#pragma once

#include <memory>
#include "solar/json/json_object.h"
#include "solar/utility/checksum.h"

namespace solar {

	class peon;

	enum class build_file_is_forced { NO, YES };

	class builder {
	public:
		virtual std::string to_string() const = 0;
		virtual std::unique_ptr<builder> clone(json_object& object) = 0;
		virtual std::string get_src_extension() = 0;
		virtual std::string get_dst_folder() = 0; //required to guarantee all dst folders exist. can be empty string if no dst folder required.
		virtual bool build_file(peon& peon, const std::string& src_path, build_file_is_forced is_forced) = 0;
	};

}
