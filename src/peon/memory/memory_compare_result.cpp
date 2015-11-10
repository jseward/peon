#include "memory_compare_result.h"
#include "solar/utility/enum_helpers.h"

namespace solar {

	const char* to_string(memory_compare_result e) {
		switch (e) {
			RETURN_ENUM_STRING(memory_compare_result, NO_DIFFERENCES);
			RETURN_ENUM_STRING(memory_compare_result, NO_MEMORY_FOUND);
			RETURN_ENUM_STRING(memory_compare_result, SRC_FILE_COUNT_CHANGED);
			RETURN_ENUM_STRING(memory_compare_result, SRC_FILE_CHANGED);
			RETURN_ENUM_STRING(memory_compare_result, DST_FILE_COUNT_CHANGED);
			RETURN_ENUM_STRING(memory_compare_result, DST_FILE_NOT_FOUND);
			RETURN_ENUM_STRING(memory_compare_result, DST_FILE_CHANGED);
			RETURN_ENUM_STRING(memory_compare_result, BUILDER_CHECKSUM_CHANGED);
			END_ENUM_STRINGS(e);
		}
	}

	std::ostream& operator<<(std::ostream& os, memory_compare_result e) {
		return os << to_string(e);
	}

}