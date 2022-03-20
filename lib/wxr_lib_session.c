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

bool wxr_session_init(wxr_session *s, wxr_date date,
			     const char *text, size_t text_len, GError **error)
{
	s->date = date;
	s->text = text;
	s->text_len = text_len;

	const char *p = text;
	const char *e = text + text_len;
	g_autofree char *exercise = NULL;

	while (p<e) {
		int rc, n;

		if (*p == '@') {
			float weight;
			rc = sscanf(p, "@ %f bw\n%n", &weight, &n);
			if (rc == 1 && n>6) {
				s->body_weight = weight;
				printf("body_weight = %f\n", weight);
				p += n;
				continue;
			}
		}

		if (*p == '#') {
			const char *q = p+1;
			while (q<e && *q != '\n')
				q++;

			n = q-p-1;
			exercise = g_strndup(p+1, n);
			printf("exercise = %s\n", exercise);

			g_free(exercise);
			exercise=NULL;

			p += n;
			continue;
		}

		if (isdigit(*p)) {

			n = wxr_session_add_line(s, p, e, error);
			if (n < 0)
				return false;

			if (n > 0)
				p += n;
		}

		/* skip this line */
		while (p<e && *p != '\n' && *p != '\r') p++;
		while (p<e && (*p == '\n' || *p == '\r')) p++;
	}

	return true;
}
void wxr_session_cleanup(wxr_session *session)
{
}

int wxr_session_add_line(wxr_session *s, const char *text,
			 const char *e, GError **error)
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


	for (unsigned i=0; i<g; i++) {
		printf(" - i=%u (%u)\n", i, tokc[i]);
		for (unsigned j=0; j<tokc[i]; j++) {
			if (i==0)
				printf("   - [%u][%u] %f\n", i, j, nums[i][j].f);
			else
				printf("   - [%u][%u] %u\n", i, j, nums[i][j].u);
		}
	}

	return p-text;
}
