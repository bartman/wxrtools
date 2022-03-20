#pragma once

#include "wxr_ctx.h"
#include "wxr_types.h"

extern bool wxr_lift_init(wxr_lift *lift, const char *name, GError **error);
extern void wxr_lift_cleanup(wxr_lift *lift);

extern int wxr_lift_add_line(wxr_lift *lift, const char *p, const char *e,
			     GError **error);
extern bool wxr_lift_add_w_r_s(wxr_lift *lift, float weight, unsigned reps,
			       unsigned sets, GError **error);

