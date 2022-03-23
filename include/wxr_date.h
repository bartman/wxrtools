/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_types.h"

extern int wxr_date_to_tm(wxr_date date, struct tm *tm, GError **error);
extern int wxr_date_to_time(wxr_date date, time_t *t, GError **error);
extern long wxr_date_diff_seconds(wxr_date first, wxr_date second, GError **error);
extern int wxr_date_diff_days(wxr_date first, wxr_date second, GError **error);

extern int wxr_date_number(wxr_date date);
extern int wxr_date_to_str(wxr_date date, char *str, size_t size);
