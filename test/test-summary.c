#include <glib.h>
#include <stdio.h>

#include "wxr.h"

struct state {
	const wxr_session *pses;
	const wxr_lift *plift;
	const wxr_entry *pent;

	wxr_date first;

	unsigned sessions;
	unsigned lifts;
	unsigned entries;

	size_t sets;
	size_t reps;
	double tonnage;
};

long process(const wxr_ctx *wxr,
	     const wxr_session *ses,
	     const wxr_lift *lift,
	     const wxr_entry *ent,
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

	float days = wxr_date_diff_days(st.pses->date, st.first, &error);
	float weeks = days / 7.0;
	float months = days / (365.25/12);
	printf("                         %11.1f days    %11.1f weeks %11.1f months\n", days, weeks, months);
	printf("sessions   = %11.1f %11.1f/day     %11.1f/week  %11.1f/month\n",
	       (float)st.sessions, st.sessions/days, st.sessions/weeks, st.sessions/months);

#define show(name) \
	printf("%-10s = %11.1f %11.1f/session %11.1f/week  %11.1f/month\n", \
	       __stringify(name), \
	       (float)st.name, \
	       (float)st.name/st.sessions, \
	       (float)st.name/weeks, \
	       (float)st.name/months)

	show(lifts);
	show(entries);

	show(sets);
	show(reps);
	show(tonnage);

	wxr_ctx_close(wxr);

	return 0;
}
