#include <glib.h>
#include <stdio.h>

#include "wxr.h"

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;

	g_assert_cmpint(argc, ==, 2);

	wxr = wxr_ctx_open(argv[1], &error);

	const wxr_index *index = wxr_ctx_get_index(wxr, &error);
	if (!index)
		g_error("%s", error->message);

	size_t count = wxr_ctx_session_count(wxr);
	printf("session count %zu\n", count);

	wxr_index_for_each_session(index, i, ses) {
		printf("%zu %04u-%02u-%2u @%g\n", i,
		       ses->date.year, ses->date.month, ses->date.day,
		       ses->body_weight);
		wxr_session_for_each_lift(ses, j, lift) {
			printf("  %zu #%s\n", j, lift->name);
			wxr_lift_for_each_entry(lift, k, ent) {
				printf("    %zu %g x %u x %u\n", k,
				       ent->weight, ent->reps, ent->sets);
			}
		}
	}

	wxr_ctx_close(wxr);

	return 0;
}
