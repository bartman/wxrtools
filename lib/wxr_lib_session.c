#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "wxr.h"

#include "wxr_lib_session.h"
#include "wxr_lib_lift.h"

#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"

bool wxr_session_init(wxr_session *ses, wxr_date date,
			     const char *text, size_t text_len, GError **error)
{
	memset(ses, 0, sizeof(*ses));

	ses->date = date;
	ses->text = text;
	ses->text_len = text_len;

	const char *p = text;
	const char *e = text + text_len;
	wxr_lift *lift = NULL;

	while (p<e) {
		int rc, n;

		if (*p == '@') {
			float weight;
			rc = sscanf(p, "@ %f bw\n%n", &weight, &n);
			if (rc == 1 && n>6) {
				ses->body_weight = weight;
#if 0
				printf("body_weight = %f\n", weight);
#endif
				p += n;
				continue;
			}
		}
		else if (*p == '#') {
			const char *q = p+1, *r = q;
			while (q<e && *q != '\n') {
				if (!isspace(*q)) r=q+1;
				q++;
			}

			n = r-p;
			if (n > 1) {
				g_autofree char *exercise = g_strndup(p, n);
				lift = wxr_session_add_lift(ses, exercise, error);
				if (!lift)
					return false;
			}

			p = q+1;
			continue;
		}
		else if (lift && isdigit(*p)) {

			n = wxr_lift_add_line(lift, p, e, error);
			if (n < 0)
				return false;

			if (n > 0)
				p += n;

		} else {
			lift = NULL;
		}

		/* skip this line */
		while (p<e && *p != '\n' && *p != '\r') p++;
		while (p<e && (*p == '\n' || *p == '\r')) p++;
	}

	return true;
}
void wxr_session_cleanup(wxr_session *ses)
{
	for (size_t i=0; i<ses->count; i++) {
		wxr_lift_cleanup(ses->lifts + i);
	}
	g_free(ses->lifts);
	ses->lifts = NULL;
	ses->size = ses->count = 0;
}

wxr_lift * wxr_session_add_lift(wxr_session *ses, const char *exercise,
				GError **error)
{
	if (ses->count >= ses->size) {
		/* need to grow */
		size_t new_size = ses->size + 128;
		size_t total_size = new_size * sizeof(*ses->lifts);
		gpointer new = g_realloc(ses->lifts, total_size);
		if (!new) {
			wxr_set_error_errno(error, "realloc session (%zu)",
					    new_size);
			return NULL;
		}
		ses->size = new_size;
		ses->lifts = new;
	}

	wxr_lift *lift = ses->lifts + ses->count;

	bool ok = wxr_lift_init(lift, exercise, error);
	if (!ok)
		return NULL;

	ses->count ++;
	return lift;
}

float wxr_session_total_volume(const wxr_session *ses)
{
	float volume = 0.0;

	wxr_session_for_each_lift(ses, i, lift)
		volume += wxr_lift_total_volume(lift);

	return volume;
}

int wxr_session_fprintf(FILE *out, const wxr_session *ses)
{
	return fprintf(out, "Session { %04u-%02u-%02u @ %.1f }",
		       ses->date.year, ses->date.month, ses->date.day,
		       ses->body_weight);
}
