#include <glib.h>
#include <stdio.h>

#include "wxr.h"

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;

	g_assert_cmpint(argc, ==, 2);

	wxr = wxr_ctx_open(argv[1], &error);

#if 0
	const char *raw;
	size_t size;
	wxr_ctx_get_contents(wxr, &raw, &size);
	write(1, raw, size);
#endif

	wxr_ctx_build_index(wxr, &error);

	size_t count = wxr_ctx_session_count(wxr);

	printf("session count %zu\n", count);

	wxr_ctx_close(wxr);

	return 0;
}
