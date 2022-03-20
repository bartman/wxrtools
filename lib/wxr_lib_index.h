#pragma once

#include "wxr_ctx.h"

struct wxr_index_s {
};
typedef struct wxr_index_s wxr_index;

extern void wxr_index_init(wxr_index *index);
extern void wxr_index_cleanup(wxr_index *index);

extern bool wxr_index_add(wxr_index *index, wxr_date date,
			  const char *str, size_t size, GError **error);
