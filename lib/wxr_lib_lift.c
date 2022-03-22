#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "wxr.h"

#include "wxr_lib_lift.h"
#include "wxr_lib_entry.h"

#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"

bool wxr_lift_init(wxr_lift *lift, const char *name, GError **error)
{
	memset(lift, 0, sizeof(*lift));
	lift->name = g_strdup(name);
	g_assert_nonnull(lift->name);
	return true;
}

void wxr_lift_cleanup(wxr_lift *lift)
{
	g_free((void*)lift->name);
	g_free(lift->entries);
	lift->entries = NULL;
	lift->size = lift->count = 0;
}

int wxr_lift_add_line(wxr_lift *lift, const char *text, const char *e,
			     GError **error)
{
	const char *p = text;
	bool ok;

#if 0
	const char *le = p;
	while (le<e && !isvspace(*le))
		le++;
	printf("# %.*s\n", (int)(le-p), p);

#endif

	/* split line into words separated by x's
	 * W,W,W x R,R,R x S,S,S */
#define MAXG 4
#define MAXN 10
	const char *toks[MAXG][MAXN] = {{NULL,},};
	unsigned tokc[MAXG] = {0,};
	unsigned g = 0;

	while (p<e) {
		/* skip leading white space */
		while (p<e && ishspace(*p))
			p++;

		/* end of line */
		if (isvspace(*p))
			break;
		if (p>=e || isvspace(*p))
			break;

		/* number */
		if (isdigit(*p)) {
			toks[g][tokc[g]++] = p++;

			while (p<e && isdigit(*p))
				p++;

			while (p<e && ishspace(*p))
				p++;

			if (p>=e || isvspace(*p))
				break;

			if (((p+2)<e) && *p == ',' && (ishspace(p[1])
						       || isdigit(p[1]))) {
				p++;
				if (tokc[g]>=MAXN)
					break;
				continue;
			}

			if (((p+2)<e) && *p == 'x' && (ishspace(p[1])
						       || isdigit(p[1]))) {
				p++;
				g ++;
				if (g>=MAXG)
					break;
				continue;
			}

			break;
		}

		break;
	}
	if (tokc[g])
		g++;

	union { float f; unsigned u; } nums[MAXG][MAXN];

	for (unsigned i=0; i<g; i++) {
		for (unsigned j=0; j<tokc[i]; j++) {
			const char *q = toks[i][j];
			while (q<e && isdigit(*q)) q++;
			if (i==0)
				nums[i][j].f = atof(toks[i][j]);
			else
				nums[i][j].u = atoi(toks[i][j]);
		}
	}


	switch (g) {
	case 0:
		g_assert_not_reached();
		return -1;

	case 1:
		nums[g][tokc[g]++].u = 1;
		g++;
	case 2:
		nums[g][tokc[g]++].u = 1;
		g++;
		break;
	}

	g_assert_cmpint(g, ==, 3);

	for (unsigned w=0; w<tokc[0]; w++) {
		float weight = nums[0][w].f;
		for (unsigned r=0; r<tokc[1]; r++) {
			unsigned reps = nums[1][r].u;
			for (unsigned s=0; s<tokc[2]; s++) {
				unsigned sets = nums[2][s].u;

				ok = wxr_lift_add_w_r_s(lift, weight, reps,
							sets, error);
				if (!ok)
					return -1;
			}
		}
	}

	return p-text;
}

bool wxr_lift_add_w_r_s(wxr_lift *lift, float weight, unsigned reps,
			unsigned sets, GError **error)
{
	if (lift->count >= lift->size) {
		/* need to grow */
		size_t new_size = lift->size + 128;
		size_t total_size = new_size * sizeof(*lift->entries);
		gpointer new = g_realloc(lift->entries, total_size);
		if (!new) {
			wxr_set_error_errno(error, "realloc lift (%zu)",
					    new_size);
			return false;
		}
		lift->size = new_size;
		lift->entries = new;
	}

	wxr_entry *ent = lift->entries + lift->count;

	bool ok = wxr_entry_init(ent, weight, reps, sets, error);
	if (!ok)
		return false;

	lift->count ++;
	return true;
}

float wxr_lift_best_1rm(const wxr_lift *lift, const wxr_entry **out_ent)
{
	const wxr_entry *best_ent = NULL;
	float best_1rm = 0;

	wxr_lift_for_each_entry(lift, i, ent) {
		float ent_1rm = wxr_entry_1rm(ent);
		if (best_1rm > ent_1rm) {
			best_1rm = ent_1rm;
			best_ent = ent;
		}
	}

	if (out_ent)
		*out_ent = best_ent;

	return best_1rm;
}

int wxr_lift_fprintf(FILE *out, const wxr_lift *lift)
{
	return fprintf(out, "Lift { %s }",
		       lift->name);
}
