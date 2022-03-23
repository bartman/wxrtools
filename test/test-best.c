#include <glib.h>
#include <stdio.h>

#include "wxr.h"

struct state {
	const char * match;

	float best_1rm;
	const wxr_session *best_ses;
	const wxr_lift    *best_lift;
	const wxr_entry   *best_ent;
};

static long filter_ses(const wxr_ctx *wxr, const wxr_session *ses,
		       void *opaque, GError **error)
{
	//struct state *st = opaque;

	return 1;
}
static long filter_lift(const wxr_ctx *wxr, const wxr_session *ses,
			const wxr_lift *lift, void *opaque, GError **error)
{
	struct state *st = opaque;

	if (!strstr(lift->name, st->match))
		return 0;

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
		st->best_lift = lift;
		st->best_ses  = ses;
	}

	return 1;
}

void do_one(wxr_ctx *wxr, const char *match)
{
	g_autoptr(GError) error = NULL;
	struct state st = { .match = match };
	long rc = wxr_ctx_filter_enumerate(wxr,
				   filter_ses, filter_lift,
				   process, &st, &error);
	if (rc<0)
		g_error("%s", error->message);

	printf("%ld entries considered to find best 1RM of %.1f\n",
	       rc, st.best_1rm);

	if (st.best_1rm) {
		wxr_session_fprintf(stdout, st.best_ses);
		puts("");
		wxr_lift_fprintf(stdout, st.best_lift);
		puts("");
		wxr_entry_fprintf(stdout, st.best_ent);
		puts("");
	}
}

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;

	g_assert_cmpint(argc, >=, 2);

	wxr = wxr_ctx_open(argv[1], &error);
	if (!wxr)
		g_error("%s", error->message);

	if (argc == 2) {
		puts("========== squat ==========");
		do_one(wxr, "#sq");
		puts("========== bench ==========");
		do_one(wxr, "#bp");
		puts("========== deadlift ==========");
		do_one(wxr, "#dl");
		puts("========== OHP ==========");
		do_one(wxr, "#ohp");
	} else {

		for (int i = 2; i<argc; i++)
			do_one(wxr, argv[i]);
	}

	wxr_ctx_close(wxr);

	return 0;
}
