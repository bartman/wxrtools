#pragma once

#include "wxr_types.h"

#include "wxr_lib_index.h"

struct wxr_ctx_s {
	const char *file_name;
	int fd;
	size_t file_size;
	void *map;

	bool indexed;
	wxr_index index;
};

extern bool wxr_ctx_build_index(wxr_ctx *wxr, GError **error);

