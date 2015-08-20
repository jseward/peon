#pragma once

#include <unordered_map>
#include "solar/json/json_object.h"
#include "builder.h"
#include "builder_type.h"

namespace solar {

	class file_system;
	class exe_runner;

	class builder_factory {
	private:
		std::unordered_map<builder_type, std::unique_ptr<builder>> _builder_templates;

	public:
		builder_factory(file_system& file_system, exe_runner& exe_runner);

		std::unique_ptr<builder> create_builder(json_object& object) const;
	};

}