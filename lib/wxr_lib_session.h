#pragma once

#include "wxr_ctx.h"
#include "wxr_types.h"

extern bool wxr_session_init(wxr_session *session, wxr_date date,
			     const char *text, size_t text_len, GError **error);
extern void wxr_session_cleanup(wxr_session *session);

extern int wxr_session_add_line(wxr_session *s, const char *p,
				const char *e, GError **error);
