/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include <glib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct wxr_ctx_s wxr_ctx;

typedef union wxr_date_u {
	uint32_t word;
	struct {
		int16_t year;
		int8_t  month;
		int8_t  day;
	};
} wxr_date;
