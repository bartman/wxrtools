#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "wxr_lib_session.h"

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

int wxr_lift_add_line(wxr_lift *s, const char *text, const char *e,
			     GError **error)
{
	const char *p = text;

#if 1
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
				printf("%f x %u x %u\n",
				       weight, reps, sets);
			}
		}
	}

	return p-text;
}

