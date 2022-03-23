#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "wxr.h"

#include "blot.h"

#define FATAL_ERROR(error) ({ \
	if (unlikely (error)) \
		g_error("%s:%u: %s", __func__, __LINE__, (error)->message); \
})

struct record {
	const wxr_session *ses;
	const wxr_lift    *lift;

	float volume;           // total tonnage
	float efficiency;       // best 1RM
	float intensity;        // highest weight lifted
	unsigned reps;          // number of reps
};

static inline bool record_used(struct record *rec)
{
	return rec->reps;
}

struct state {
	wxr_date d0, d1;
	const char * match;

	const wxr_session *curr_ses;
	const wxr_lift    *curr_lift;

	struct record     *curr_rec;

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
	if (st->curr_rec && !record_used(st->curr_rec))
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
	if (!rec || record_used(rec))
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

	if (!ent->reps || !ent->sets)
		return 0;

	struct record *rec = st->curr_rec;
	bool addit = !record_used(rec);

	rec->reps   += ent->reps * ent->sets;
	rec->volume += wxr_entry_total_volume(ent);

	float ent_1rm = wxr_entry_1rm(ent);
	if (rec->efficiency < ent_1rm)
		rec->efficiency = ent_1rm;

	if (rec->intensity < ent->weight)
		rec->intensity = ent->weight;

	g_assert_cmpint((uintptr_t)rec->ses, ==, (uintptr_t)st->curr_ses);
	g_assert_cmpint((uintptr_t)rec->lift, ==, (uintptr_t)st->curr_lift);

	if (addit) {
		st->records = g_list_append(st->records, rec);
		st->count ++;
	}

	return 1;
}

int record_comar_date(const void *va, const void *vb)
{
	g_autoptr(GError) error = NULL;
	const struct record *a = va;
	const struct record *b = vb;
	return wxr_date_diff_days(a->ses->date, b->ses->date, &error);
}

void plot_one(blot_color col, const char *name, blot_data_type data_type,
	      size_t count, void *x, void *y,
	      size_t xlabel_count, char **xlabels,
	      int cols, int rows)
{
	g_autoptr(GError) error = NULL;

	blot_render_flags flags = 0;
	flags |= BLOT_RENDER_BRAILLE;
	//flags |= BLOT_RENDER_LEGEND_BELOW;
	flags |= BLOT_RENDER_LEGEND_ABOVE;
	//flags |= BLOT_RENDER_NO_X_AXIS;
	//flags |= BLOT_RENDER_NO_Y_AXIS;

	blot_figure *fig;

	fig = blot_figure_new(&error);
	FATAL_ERROR(error);

	blot_figure_set_axis_color(fig, 8, &error);
	FATAL_ERROR(error);

	blot_figure_set_screen_size(fig, cols, rows, &error);
	FATAL_ERROR(error);

	blot_figure_set_x_axis_labels(fig, xlabel_count, xlabels, &error);
	FATAL_ERROR(error);

	blot_figure_line(fig, data_type,
			 count, x, y,
			 col, name, &error);
	FATAL_ERROR(error);

	blot_screen *scr = blot_figure_render(fig, flags, &error);
	FATAL_ERROR(error);

	gsize txt_size = 0;
	const wchar_t *txt = blot_screen_get_text(scr, &txt_size, &error);
	FATAL_ERROR(error);

	printf("%ls", txt);

	blot_screen_delete(scr);
	blot_figure_delete(fig);
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

	printf("count = %u\n", st.count);

	st.records = g_list_sort(st.records, record_comar_date);

	int cols, rows;
	blot_terminal_get_size(&cols, &rows, &error);
	FATAL_ERROR(error);

	int      *date        = g_malloc_n(st.count, sizeof(int));
	float    *body_weight = g_malloc_n(st.count, sizeof(float));
	float    *volume      = g_malloc_n(st.count, sizeof(float));
	float    *efficiency  = g_malloc_n(st.count, sizeof(float));
	float    *intensity   = g_malloc_n(st.count, sizeof(float));
	unsigned *reps        = g_malloc_n(st.count, sizeof(unsigned));

	size_t xlabel_count = (cols*0.8)/15;
	char    **datelabels  = g_malloc_strv(st.count, 16);

	if (xlabel_count>st.count)
		xlabel_count = st.count;

	unsigned d = 0;
	unsigned i = 0;
	for (GList *e = g_list_first(st.records); e; e = e->next, i++) {
		struct record *rec = e->data;

		time_t t;
		wxr_date_to_time(rec->ses->date, &t, &error);
		FATAL_ERROR(error);
		date[i] = t;

		if ((i % ((st.count+xlabel_count-1)/xlabel_count)) == 0) {
			rc = wxr_date_to_str(rec->ses->date, datelabels[d], 15);
			d++;
		}

		body_weight[i] = rec->ses->body_weight;
		volume[i] = rec->volume;
		efficiency[i] = rec->efficiency;
		intensity[i] = rec->intensity;
		reps[i] = rec->reps;
	}

	rows = (rows / 5) - 1;

	/* --------- */

	plot_one(8, "body_weight", BLOT_DATA_(INT32, FLOAT),
		 st.count, date, body_weight,
		 xlabel_count, datelabels,
		 cols, rows);

	plot_one(9, "volume", BLOT_DATA_(INT32, FLOAT),
		 st.count, date, volume,
		 xlabel_count, datelabels,
		 cols, rows);

	plot_one(10, "efficiency", BLOT_DATA_(INT32, FLOAT),
		 st.count, date, efficiency,
		 xlabel_count, datelabels,
		 cols, rows);

	plot_one(11, "intensity", BLOT_DATA_(INT32, FLOAT),
		 st.count, date, intensity,
		 xlabel_count, datelabels,
		 cols, rows);

	plot_one(12, "reps", BLOT_DATA_(INT32, INT32),
		 st.count, date, reps,
		 xlabel_count, datelabels,
		 cols, rows);

	/* --------- */

	g_free(date);
	g_free(datelabels);
	g_free(volume);
	g_free(efficiency);
	g_free(intensity);
	g_free(reps);

	state_cleanup(&st);
}

int main(int argc, char * argv[])
{
	g_autoptr(GError) error = NULL;
	wxr_ctx *wxr;
	int rc;

	g_assert_cmpint(argc, >=, 2);

	setlocale(LC_CTYPE, "");

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
