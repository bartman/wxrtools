#pragma once

#include "wxr_ctx.h"
#include "wxr_types.h"

extern void wxr_index_init(wxr_index *index);
extern void wxr_index_cleanup(wxr_index *index);

extern bool wxr_index_add(wxr_index *index, wxr_date date,
			  const char *text, size_t text_len, GError **error);
