#pragma once

#include "wxr_types.h"

struct wxr_ctx {
	const char *file_name;
	int fd;
	size_t file_size;
	void *map;
};


