#include <glib.h>
#include <stdio.h>
#include <err.h>

#include "wxr.h"
#include "../lib/wxr_lib_session.h"

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_date date = { .year = 2022, .month = 3, .day = 20 };
	wxr_session s;
	bool ok;

	const char *text =
		"#squat #sq\n"
		"50 x 5 x 5\n"
		"\n"
		"#OHP\n"
		"60 x 5 x 5\n"
		"\n"
		"#deadlift #dl\n"
		"100 x 5\n"
		"\n";

	ok = wxr_session_init(&s, date, text, strlen(text), &error);
	if (!ok)
		g_error("%s\n", error->message);

	return 0;
}
