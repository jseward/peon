#include "builder_type.h"
#include "solar/utility/enum_helpers.h"

namespace solar {

	const char* to_string(builder_type e) {
		switch (e) {
			RETURN_ENUM_STRING(builder_type, COPY);
			RETURN_ENUM_STRING(builder_type, EXEC);
			RETURN_ENUM_STRING(builder_type, DEPENDENCY_FILE);
			RETURN_ENUM_STRING(builder_type, BUILD_CONFIG_FILE);
			END_ENUM_STRINGS(e);
		}
	}

}
