#include <glib.h>
#include <time.h>
#include <stdio.h>

#include "wxr_date.h"
#include "wxr_types.h"
#include "wxr_error.h"

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
		     "mktime(%04u-%02u-%02u)",
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
	return snprintf(str, size,
			"%04u-%02u-%02u",
			date.year, date.month, date.day);
}
