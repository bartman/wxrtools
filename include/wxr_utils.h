/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

#include "blot_utils.h"

/* rounds an integer (N) up to the next multiple of (M) */
#define ROUND_INT_UP_TO(N,M) \
	((N) + (M) - 1 - (((N)+(M)-1) % (M)))

static inline bool ishspace(char ch)
{
	switch (ch) {
	case ' ':
	case '\t':
		return true;
	default:
		return false;
	}
}

static inline bool isvspace(char ch)
{
	switch (ch) {
	case '\n':
	case '\r':
	case '\v':
		return true;
	default:
		return false;
	}
}

/* allocates a single buffer that contains an array of strings pointers, and
 * the strings themselves. the entire structure is freed with g_free() after */
extern char ** g_malloc_strv(size_t count, size_t size);
