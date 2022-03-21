/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

extern wxr_ctx* wxr_ctx_open(const char *file_name, GError **error);
extern void wxr_ctx_close(wxr_ctx *wxr);

extern void wxr_ctx_get_contents(wxr_ctx *wxr,
				 const char **ptr, size_t *size);

extern const wxr_index * wxr_ctx_get_index(wxr_ctx *wxr, GError **error);
extern size_t wxr_ctx_session_count(wxr_ctx *wxr);

typedef long (*wxr_enumerate_ent_cb_fn)(wxr_ctx *wxr, wxr_session *ses,
					wxr_lift *lift, wxr_entry *ent,
					void *opaque, GError **error);
extern long wxr_ctx_enumerate(wxr_ctx *wxr, wxr_enumerate_ent_cb_fn,
			      void *opaque, GError **error);

typedef long (*wxr_filter_ses_cb_fn)(wxr_ctx *wxr, wxr_session *ses,
					void *opaque, GError **error);
typedef long (*wxr_filter_lift_cb_fn)(wxr_ctx *wxr, wxr_lift *lift,
					void *opaque, GError **error);
typedef long (*wxr_filter_ent_cb_fn)(wxr_ctx *wxr, wxr_entry *ent,
					void *opaque, GError **error);
extern long wxr_ctx_filter_enumerate(wxr_ctx *wxr,
			      wxr_filter_ses_cb_fn,
			      wxr_filter_lift_cb_fn,
			      wxr_filter_ent_cb_fn,
			      wxr_enumerate_ent_cb_fn,
			      void *opaque, GError **error);
