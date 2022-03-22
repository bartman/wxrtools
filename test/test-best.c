#include <glib.h>
#include <stdio.h>

#include "wxr.h"

struct state {
	const char * match;

	const wxr_session *curr_ses;
	const wxr_lift    *curr_lift;

	float best_1rm;
	const wxr_session *best_ses;
	const wxr_lift    *best_lift;
	const wxr_entry   *best_ent;
};

static long filter_ses(const wxr_ctx *wxr, const wxr_session *ses,
		       void *opaque, GError **error)
{
	struct state *st = opaque;

	st->curr_ses = ses;

	return 1;
}
static long filter_lift(const wxr_ctx *wxr, const wxr_lift *lift,
			void *opaque, GError **error)
{
	struct state *st = opaque;

	if (!strstr(lift->name, st->match))
		return 0;

	st->curr_lift = lift;

	return 1;
}
long process(const wxr_ctx *wxr,
	     const wxr_session *ses,
	     const wxr_lift *lift,
	     const wxr_entry *ent,
	     void *opaque,
	     GError **error)
{
	struct state *st = opaque;

	float ent_1rm = wxr_entry_1rm(ent);

	if (st->best_1rm < ent_1rm) {
		st->best_1rm  = ent_1rm;
		st->best_ent  = ent;
		st->best_lift = st->curr_lift;
		st->best_ses  = st->curr_ses;
	}

	return 1;
}

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;

	g_assert_cmpint(argc, ==, 3);

	wxr = wxr_ctx_open(argv[1], &error);
	if (!wxr)
		g_error("%s", error->message);

	struct state st = { .match = argv[2] };
	long rc = wxr_ctx_filter_enumerate(wxr,
				   filter_ses, filter_lift,
				   process, &st, &error);

	if (rc<0)
		g_error("%s", error->message);

	printf("rc=%ld\n", rc);
	printf("best_1rm = %.1f\n", st.best_1rm);

	if (st.best_1rm) {
		wxr_session_fprintf(stdout, st.best_ses);
		puts("");
		wxr_lift_fprintf(stdout, st.best_lift);
		puts("");
		wxr_entry_fprintf(stdout, st.best_ent);
		puts("");
	}

	wxr_ctx_close(wxr);

	return 0;
}
