/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

extern wxr_ctx* wxr_ctx_open(const char *file_name, GError **error);
extern void wxr_ctx_close(wxr_ctx *wxr);

extern void wxr_ctx_get_contents(wxr_ctx *wxr,
				 const char **ptr, size_t *size);

extern bool wxr_ctx_build_index(wxr_ctx *wxr, GError **error);
