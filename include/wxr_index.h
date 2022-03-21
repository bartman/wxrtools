/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

#define wxr_index_for_each_session(_index, _i, _ses) \
	for(size_t _i = 0; _i < (_index)->count; _i++) \
	for(wxr_session *_ses = (_index)->sessions + _i; _ses; _ses = NULL)

