/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include <stdio.h>

#include "wxr_types.h"

#define wxr_lift_for_each_entry(_lift, _i, _ent) \
	for(size_t _i = 0; _i < (_lift)->count; _i++) \
	for(const wxr_entry *_ent = (_lift)->entries + _i; _ent; _ent = NULL)

extern float wxr_lift_best_1rm(const wxr_lift *lift, const wxr_entry **entry);

extern float wxr_lift_total_volume(const wxr_lift *lift);

extern int wxr_lift_fprintf(FILE *out, const wxr_lift *lift);
