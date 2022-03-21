/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

extern wxr_ctx* wxr_ctx_open(const char *file_name, GError **error);
extern void wxr_ctx_close(wxr_ctx *wxr);

extern void wxr_ctx_get_contents(wxr_ctx *wxr,
				 const char **ptr, size_t *size);

extern const wxr_index * wxr_ctx_get_index(wxr_ctx *wxr, GError **error);
extern size_t wxr_ctx_session_count(wxr_ctx *wxr);
