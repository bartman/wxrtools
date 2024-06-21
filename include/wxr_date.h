/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

#define WXR_DATE_FORMAT "%04hu-%02hhu-%02hhu"
#define WXR_DATE_SIZE   12

extern wxr_date wxr_date_today(GError **error);
extern int wxr_date_to_tm(wxr_date date, struct tm *tm, GError **error);
extern int wxr_date_to_time(wxr_date date, time_t *t, GError **error);
extern long wxr_date_diff_seconds(wxr_date first, wxr_date second, GError **error);
extern int wxr_date_diff_days(wxr_date first, wxr_date second, GError **error);

extern uint64_t wxr_date_number(wxr_date date);
extern int wxr_date_to_str(wxr_date date, char *str, size_t size);
#define wxr_date_str(_date) ({ \
	char *_buf = alloca(WXR_DATE_SIZE); \
	_buf[0] = 0; \
	int _rc = wxr_date_to_str(_date, _buf, WXR_DATE_SIZE); \
	if (_rc<0) strcpy(_buf, "<bad>"); \
	_buf; \
})
extern int wxr_date_compare(wxr_date first, wxr_date second);

extern int wxr_date_parse_loosely(wxr_date *date, const char *str, bool end);

extern int wxr_date_range_parse_loosely(wxr_date *d0, wxr_date *d1, const char *str);
