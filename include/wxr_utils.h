/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

#include "blot_utils.h"

/* rounds an integer (N) up to the next multiple of (M) */
#define ROUND_INT_UP_TO(N,M) \
	((N) + (M) - 1 - (((N)+(M)-1) % (M)))

