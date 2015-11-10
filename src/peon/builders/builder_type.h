#pragma once

namespace solar {

	enum class builder_type {
		invalid = -1,

		COPY,
		EXEC,
		DEPENDENCY_FILE,
		
		count
	};

	extern const char* to_string(builder_type e);

}