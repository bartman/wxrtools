#include <glib.h>
#include <time.h>

#include "wxr_date.h"
#include "wxr_types.h"
#include "wxr_error.h"

int wxr_date_diff_days(wxr_date first, wxr_date second, GError **error)
{
	struct tm tm1 = { .tm_mday = first.day,
			  .tm_mon  = first.month - 1,
			  .tm_year = first.year - 1900 },
		  tm2 = { .tm_mday = second.day,
			  .tm_mon  = second.month - 1,
			  .tm_year = second.year - 1900
		  };

	time_t t1 = mktime(&tm1);
	RETURN_ERROR(t1 == (time_t)-1, -1, error,
		     "mktime(%04u-%02u-%02u)",
		     first.year, first.month, first.day);

	time_t t2 = mktime(&tm2);
	RETURN_ERROR(t1 == (time_t)-1, -1, error,
		     "mktime(%04u-%02u-%02u)",
		     second.year, second.month, second.day);

	long diff = t1 - t2;

	return diff / (24L*60*60);
}
