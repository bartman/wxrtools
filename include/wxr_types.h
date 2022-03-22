/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include <glib.h>
#include <stdint.h>
#include <stdbool.h>

#include "wxr_config.h"

typedef struct wxr_ctx_s wxr_ctx;

typedef union wxr_date_u {
	uint32_t word;
	struct {
		int16_t year;
		int8_t  month;
		int8_t  day;
	};
} wxr_date;

typedef struct wxr_entry_s {
	float weight;
	uint16_t reps;
	uint16_t sets;
} wxr_entry;

typedef struct wxr_lift_s {
	const char *name;

	size_t size;
	size_t count;
	wxr_entry *entries;
} wxr_lift;

typedef struct wxr_session_s {
	wxr_date date;

	const char *text;
	size_t text_len;

	float body_weight;

	size_t size;
	size_t count;
	wxr_lift *lifts;
} wxr_session;

typedef struct wxr_index_s {
	size_t size;
	size_t count;
	wxr_session *sessions;
} wxr_index;
