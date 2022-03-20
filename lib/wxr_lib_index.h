#pragma once

#include "wxr_ctx.h"
#include "wxr_types.h"

struct wxr_index_s {
	size_t size;
	size_t count;

	wxr_session *sessions;
};
typedef struct wxr_index_s wxr_index;

extern void wxr_index_init(wxr_index *index);
extern void wxr_index_cleanup(wxr_index *index);

extern bool wxr_index_add(wxr_index *index, wxr_date date,
			  const char *text, size_t text_len, GError **error);
