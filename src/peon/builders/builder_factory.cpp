#include "builder_factory.h"

#include "solar/utility/alert.h"
#include "solar/utility/enum_helpers.h"
#include "builder_type.h"
#include "copy_builder.h"
#include "exec_builder.h"
#include "dependency_file_builder.h"

namespace solar {

	builder_factory::builder_factory(file_system& file_system, exe_runner& exe_runner) {
		_builder_templates[builder_type::COPY] = std::make_unique<copy_builder>(file_system);
		_builder_templates[builder_type::EXEC] = std::make_unique<exec_builder>(file_system, exe_runner);
		_builder_templates[builder_type::DEPENDENCY_FILE] = std::make_unique<dependency_file_builder>(file_system);
	}

	std::unique_ptr<builder> builder_factory::create_builder(json_object& object) const {
		auto type = parse_enum<builder_type_details>(object.get_string("type"));
		if (type != builder_type::invalid) {
			auto& builder_template_iter = _builder_templates.find(type);
			if (builder_template_iter == _builder_templates.end()) {
				ALERT("unknown builder type : {}", builder_type_details::get_string(type));
			}
			else {
				return builder_template_iter->second->clone(object);
			}
		}
		return nullptr;
	}

}