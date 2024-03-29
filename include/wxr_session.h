/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include <stdio.h>

#include "wxr_types.h"

#define wxr_session_for_each_lift(_ses, _i, _lift) \
	for(size_t _i = 0; _i < (_ses)->count; _i++) \
	for(const wxr_lift *_lift = (_ses)->lifts + _i; _lift; _lift = NULL)

extern float wxr_session_total_volume(const wxr_session *ses);

extern int wxr_session_fprintf(FILE *out, const wxr_session *ses);
