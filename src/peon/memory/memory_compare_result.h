#pragma once

#include <ostream>

namespace solar {

	enum class memory_compare_result {
		invalid = -1,

		NO_DIFFERENCES,
		NO_MEMORY_FOUND,
		SRC_FILE_COUNT_CHANGED,
		SRC_FILE_CHANGED,
		DST_FILE_COUNT_CHANGED,
		DST_FILE_NOT_FOUND,
		DST_FILE_CHANGED,
		BUILDER_CHECKSUM_CHANGED,

		count
	};

	std::ostream& operator<<(std::ostream& os, memory_compare_result e);

	class memory_compare_result_details {
	public:
		static const char* get_string(memory_compare_result e);
	};

}