#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

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
	const char * match;

	const wxr_session *curr_ses;
	const wxr_lift    *curr_lift;

	struct record     *curr_rec;

	unsigned count;
	GList *records;
};

static void state_init(struct state *st, const char *match)
{
	memset(st, 0, sizeof(*st));

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

	struct record *rec = st->curr_rec;
	if (!rec || rec->best_ent)
		rec = st->curr_rec = g_malloc0(sizeof(struct record));

	rec->ses = st->curr_ses;
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

		g_assert_cmpint((uintptr_t)rec->ses, ==, (uintptr_t)st->curr_ses);
		g_assert_cmpint((uintptr_t)rec->lift, ==, (uintptr_t)st->curr_lift);

		if (addit) {
			st->records = g_list_append(st->records, rec);
			st->count ++;
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

void do_one(wxr_ctx *wxr, const char *match)
{
	g_autoptr(GError) error = NULL;
	struct state st;

	state_init(&st, match);

	long rc = wxr_ctx_filter_enumerate(wxr,
				   filter_ses, filter_lift,
				   process, &st, &error);
	if (rc<0)
		g_error("%s", error->message);

	printf("count = %u\n", st.count);

	st.records = g_list_sort(st.records, record_comar);

	for (GList *e = g_list_first(st.records); e; e = e->next) {
		struct record *rec = e->data;

		printf("%04u-%02u-%02u  | %.1f | %-10s | %5.1f | ",
		       rec->ses->date.year,
		       rec->ses->date.month,
		       rec->ses->date.day,
		       rec->ses->body_weight,
		       rec->lift->name,
		       rec->best_1rm);
		for (int r=1; r<=MAX_REPS; r++) {
			if (r == rec->best_ent->reps) {
				printf("%.f |",
				       rec->best_ent->weight);
				continue;
			}
			printf("%s%.f%s |",
			       fg(BRIGHT_BLACK),
			       wxr_weight_from_1rm_reps(rec->best_1rm, r),
			       COL_RESET);
		}
		puts("");
	}

	state_cleanup(&st);
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
