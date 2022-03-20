#pragma once

#include "wxr_ctx.h"
#include "wxr_types.h"

extern bool wxr_entry_init(wxr_entry *ent, float weight, unsigned reps,
			   unsigned sets, GError **error);
extern void wxr_entry_cleanup(wxr_entry *ent);
