#pragma once

namespace solar {

	enum class builder_type {
		invalid = -1,

		COPY,
		EXEC,
		DEPENDENCY_FILE,
		
		count
	};

	class builder_type_details {
	public:
		typedef builder_type enum_type;
		static const char* get_string(builder_type e);
	};

}