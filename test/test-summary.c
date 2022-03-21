#include <glib.h>
#include <stdio.h>

#include "wxr.h"

struct state {
	wxr_session *pses;
	wxr_lift *plift;
	wxr_entry *pent;

	wxr_date first;

	unsigned sessions;
	unsigned lifts;
	unsigned entries;

	size_t sets;
	size_t reps;
	double tonnage;
};

long process(wxr_ctx *wxr,
	     wxr_session *ses,
	     wxr_lift *lift,
	     wxr_entry *ent,
	     void *opaque,
	     GError **error)
{
	struct state *st = opaque;

	if (!st->pent)
		st->first = ses->date;

	if (st->pses != ses)
		st->sessions ++;
	if (st->plift != lift)
		st->lifts ++;
	if (st->pent != ent)
		st->entries ++;

	st->sets    += ent->sets;
	st->reps    += ent->reps * ent->sets;
	st->tonnage += (double)ent->weight * ent->reps * ent->sets;

	st->pses = ses;
	st->plift = lift;
	st->pent = ent;
	return 0;
}

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;

	g_assert_cmpint(argc, ==, 2);

	wxr = wxr_ctx_open(argv[1], &error);
	if (!wxr)
		g_error("%s", error->message);

	struct state st = {};
	long rc = wxr_ctx_enumerate(wxr, process, &st, &error);
	if (rc<0)
		g_error("%s", error->message);

	unsigned days = wxr_date_diff_days(st.pses->date, st.first, &error);

	printf("days     = %11u\n", days);

	printf("sessions = %11u %11.1f/day\n",     st.sessions, (float)st.sessions/days);
	printf("lifts    = %11u %11.1f/session\n", st.lifts,    (float)st.lifts/st.sessions);
	printf("entries  = %11u %11.1f/session\n", st.entries,  (float)st.entries/st.sessions);

	printf("sets     = %11zu %11.1f/session\n",   st.sets,    (float)st.sets/st.sessions);
	printf("reps     = %11zu %11.1f/session\n",   st.reps,    (float)st.reps/st.sessions);
	printf("tonnage  = %11.1lf %11.1f/session\n", st.tonnage, st.tonnage/st.sessions);

	wxr_ctx_close(wxr);

	return 0;
}
