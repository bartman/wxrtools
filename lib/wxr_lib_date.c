#define _GNU_SOURCE
#define _POSIX_C_SOURCE
#include <glib.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

#include "wxr_date.h"
#include "wxr_types.h"
#include "wxr_error.h"

wxr_date wxr_date_today(GError **error)
{
	wxr_date date = { .word = 0 };

	struct timeval tv;
	int rc = gettimeofday(&tv, NULL);
	RETURN_ERROR(rc<0, date, error, "gettimeofday()");

	struct tm *tm, _tm;
	tm = localtime_r(&tv.tv_sec, &_tm);
	RETURN_ERROR(!tm, date, error, "gettimeofday()");

	date.year  = tm->tm_year + 1900;
	date.month = tm->tm_mon + 1;
	date.day   = tm->tm_mday;

	return date;
}

int wxr_date_to_tm(wxr_date date, struct tm *tm, GError **error)
{
	memset(tm, 0, sizeof(*tm));
	tm->tm_mday = date.day;
	tm->tm_mon  = date.month - 1;
	tm->tm_year = date.year - 1900;
	return 0;
}

int wxr_date_to_time(wxr_date date, time_t *t, GError **error)
{
	struct tm tm = { .tm_mday = date.day,
			 .tm_mon  = date.month - 1,
			 .tm_year = date.year - 1900 };

	*t = mktime(&tm);
	RETURN_ERROR(*t == (time_t)-1, -1, error,
		     "mktime("WXR_DATE_FORMAT")",
		     date.year, date.month, date.day);

	return 0;
}

long wxr_date_diff_seconds(wxr_date first, wxr_date second, GError **error)
{
	int rc;

	time_t t1;
	rc = wxr_date_to_time(first, &t1, error);
	RETURN_IF(rc<0, rc);

	time_t t2;
	rc = wxr_date_to_time(second, &t2, error);
	RETURN_IF(rc<0, rc);

	long diff = t1 - t2;

	return diff;
}

int wxr_date_diff_days(wxr_date first, wxr_date second, GError **error)
{
	long diff = wxr_date_diff_seconds(first, second, error);
	RETURN_IF(diff<0, -1);

	return diff / (24L*60*60);
}

uint64_t wxr_date_number(wxr_date date)
{
	return ((uint64_t)date.year * 10000uL)
		+ ((uint64_t)date.month * 100uL)
		+ (uint64_t)date.day;
}

/* returns -1 if first<second, +1 if first>second, and 0 if equal */
int wxr_date_compare(wxr_date first, wxr_date second)
{
	return first.word < second.word ? -1 :
	       first.word > second.word ? +1 : 0;
}

int wxr_date_to_str(wxr_date date, char *str, size_t size)
{
	return snprintf(str, size, WXR_DATE_FORMAT,
			date.year, date.month, date.day);
}

int wxr_date_parse_loosely(wxr_date *date, const char *str, bool end)
{
	date->word = end ? -1 : 0;

	int rc = sscanf(str, WXR_DATE_FORMAT,
			&date->year, &date->month, &date->day);
#if 0
	switch (rc) {
	default:
		date->year = end ? -1 : 0;
	case 1:
		date->month = end ? -1 : 0;
	case 2:
		date->day = end ? -1 : 0;
	case 3:
		break;
	}
#endif
	return rc;
}

int wxr_date_range_parse_loosely(wxr_date *d0, wxr_date *d1, const char *str)
{
	char *ch = index(str, ':');
	if (!ch)
		return -ENOENT;

	char *s0 = strndupa(str, ch-str);
	char *s1 = ch + 1;

	wxr_date_parse_loosely(d0, s0, false);
	wxr_date_parse_loosely(d1, s1, true);
	return 0;
}
