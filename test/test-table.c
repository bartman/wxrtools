#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wxr.h"

#include "blot_color.h"

#define MAX_REPS 10

struct record {
	const wxr_session *ses;
	const wxr_lift    *lift;
	const wxr_entry   *best_ent;
	float best_1rm;
};

struct state {
	wxr_date d0, d1;
	const char * match;

	struct record     *curr_rec;

	unsigned max_lift_width;
	unsigned count;
	GList *records;
};

static void state_init(struct state *st, wxr_date d0, wxr_date d1,
		       const char *match)
{
	memset(st, 0, sizeof(*st));

	st->d0 = d0;
	st->d1 = d1;
	st->match = match;
	st->records = NULL;
}

static void state_cleanup(struct state *st)
{
	if (st->curr_rec && !st->curr_rec->best_ent)
		g_free(st->curr_rec);

	g_list_free_full(st->records, g_free);
	memset(st, 0, sizeof(*st));
}

static long filter_ses(const wxr_ctx *wxr, const wxr_session *ses,
		       void *opaque, GError **error)
{
	struct state *st = opaque;

	if (wxr_date_compare(st->d0, ses->date) >= 0)
		return 0;

	if (wxr_date_compare(st->d1, ses->date) <= 0)
		return 0;

	return 1;
}
static long filter_lift(const wxr_ctx *wxr, const wxr_session *ses,
			const wxr_lift *lift, void *opaque, GError **error)
{
	struct state *st = opaque;

	if (!strstr(lift->name, st->match))
		return 0;

	struct record *rec = st->curr_rec;
	if (!rec || rec->best_ent)
		rec = st->curr_rec = g_malloc0(sizeof(struct record));

	rec->ses = ses;
	rec->lift = lift;

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

	if (!ent->reps || ent->reps > MAX_REPS)
		return 0;

	struct record *rec = st->curr_rec;

	float ent_1rm = wxr_entry_1rm(ent);
	if (rec->best_1rm < ent_1rm) {
		bool addit = !rec->best_ent;

		rec->best_1rm  = ent_1rm;
		rec->best_ent  = ent;

		g_assert_cmpint((uintptr_t)rec->ses, ==, (uintptr_t)ses);
		g_assert_cmpint((uintptr_t)rec->lift, ==, (uintptr_t)lift);

		if (addit) {
			st->records = g_list_append(st->records, rec);
			st->count ++;

			size_t width = strlen(lift->name);
			if (st->max_lift_width < width)
				st->max_lift_width = width;
#if 0
			printf("added...\n\t");
			wxr_session_fprintf(stdout, rec->ses);
			printf("\n\t");
			wxr_lift_fprintf(stdout, rec->lift);
			printf("\n\t");
			wxr_entry_fprintf(stdout, rec->best_ent);
			puts("");
#endif
		}
	}

	return 1;
}

int record_comar(const void *va, const void *vb)
{
	const struct record *a = va;
	const struct record *b = vb;
	float diff = a->best_1rm - b->best_1rm;
	return diff < 0 ? -1 :
		diff > 0 ? 1 : 0;
}

void do_one(wxr_ctx *wxr, wxr_date d0, wxr_date d1, const char *match)
{
	g_autoptr(GError) error = NULL;
	struct state st;

	state_init(&st, d0, d1, match);

	long rc = wxr_ctx_filter_enumerate(wxr,
				   filter_ses, filter_lift,
				   process, &st, &error);
	if (rc<0)
		g_error("%s", error->message);

	printf("%s count = %u\n", match, st.count);

	st.records = g_list_sort(st.records, record_comar);

	wxr_date today = wxr_date_today(&error);
	FATAL_ERROR(error);

	int gradiant[12] = { 0x14, 0x15, 0x39, 0x5d, 0x81, 0xa5,
			     0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4 };

	int best_lifted[MAX_REPS] = { 0, };
	int best_col[MAX_REPS] = { 0, };

	int i = 0;
	for (GList *e = g_list_first(st.records); e; e = e->next, i++) {
		struct record *rec = e->data;

		int days = wxr_date_diff_days(today, rec->ses->date, &error);
		FATAL_ERROR(error);

		int g = i * 12 / st.count;
		int col = gradiant[g];

		if (days <= 7)
			col = BRIGHT_YELLOW;

		const char *coltxt = fg(col);
		const char *coldim = fg(BRIGHT_BLACK);

		int r = rec->best_ent->reps - 1;
		g_assert(r>=0 && r<MAX_REPS);
		if (best_lifted[r] < rec->best_ent->weight) {
			best_lifted[r] = rec->best_ent->weight;
			best_col[r] = col;
		}

		printf("%s%s%s | %s%4u%s | %5.1f | %-*s | %s%5.1f%s |",
		       coltxt, wxr_date_str(rec->ses->date), COL_RESET,
		       coltxt, days, COL_RESET,
		       rec->ses->body_weight,
		       st.max_lift_width,
		       rec->lift->name,
		       coltxt, rec->best_1rm, COL_RESET);
		for (int r=1; r<=MAX_REPS; r++) {
			const char *colrep = coldim;
			if (r == rec->best_ent->reps)
				colrep = coltxt;

			printf("%s%4.f%s |",
			       colrep,
			       wxr_weight_from_1rm_reps(rec->best_1rm, r),
			       COL_RESET);
		}
		puts("");
	}

	printf("%s%s%-10s | %4s | %-5s | %-*s | %5s |",
	       bg(0x11),
	       fg(0xE2),
	       "date",
	       "days",
	       "BW",
	       st.max_lift_width,
	       "lift",
	       "1RM");
	for (int r=1; r<=MAX_REPS; r++) {
		printf(" %3u |", r);
	}
	printf("%s\n", COL_RESET);

	printf("%*s |",
	       36 + st.max_lift_width,
	       "best weight lifted");
	for (int r=0; r<MAX_REPS; r++) {
		const char *colrep = fg(best_col[r]);
		printf(" %s%3u%s |", colrep, best_lifted[r], COL_RESET);
	}
	puts("");


	state_cleanup(&st);
}

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;
	int rc;

	g_assert_cmpint(argc, >=, 2);

	wxr = wxr_ctx_open(argv[1], &error);
	if (!wxr)
		g_error("%s", error->message);

	wxr_date d0={.word=0}, d1={.word=-1};

	if (argc == 2) {
		puts("========== squat ==========");
		do_one(wxr, d0, d1, "#sq");
		puts("========== bench ==========");
		do_one(wxr, d0, d1, "#bp");
		puts("========== deadlift ==========");
		do_one(wxr, d0, d1, "#dl");
		puts("========== OHP ==========");
		do_one(wxr, d0, d1, "#ohp");
	} else {

		for (int i = 2; i<argc; i++) {
			rc = wxr_date_range_parse_loosely(&d0, &d1, argv[i]);
			if (rc == 0)
				continue;

			do_one(wxr, d0, d1, argv[i]);
		}
	}

	wxr_ctx_close(wxr);

	return 0;
}
