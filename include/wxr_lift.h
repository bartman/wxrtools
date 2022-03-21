/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

#define wxr_lift_for_each_entry(_lift, _i, _ent) \
	for(size_t _i = 0; _i < (_lift)->count; _i++) \
	for(wxr_entry *_ent = (_lift)->entries + _i; _ent; _ent = NULL)


