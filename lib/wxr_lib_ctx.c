#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "wxr_lib_ctx.h"
#include "wxr.h"

#include "wxr_utils.h"
#include "wxr_error.h"

wxr_ctx* wxr_ctx_open(const char *file_name, GError **error)
{
	wxr_ctx *wxr;

	size_t total_size = sizeof(*wxr);
	wxr = g_malloc0(total_size);
	RETURN_ERROR(!wxr, NULL, error, "new wxr_ctx (%zu)", total_size);

	wxr->file_name = g_strdup(file_name);
	g_assert_nonnull(wxr->file_name);

	wxr->fd = open(wxr->file_name, O_RDONLY);
	if (wxr->fd < 0) {
		wxr_set_error_errno(error, "open(%s)", file_name);
		goto err_open;
	}

	struct stat st;
	int rc = fstat(wxr->fd, &st);
	if (rc < 0) {
		wxr_set_error_errno(error, "stat(%s)", file_name);
		goto err_stat;
	}
	wxr->file_size = st.st_size;

	size_t map_size = ROUND_INT_UP_TO(wxr->file_size+1, PAGE_SIZE);
	wxr->map = mmap(NULL, map_size, PROT_READ, MAP_SHARED,  wxr->fd, 0);
	if (wxr->map == MAP_FAILED) {
		wxr_set_error_errno(error, "mmap(%s)", file_name);
		goto err_map;
	}

	wxr_index_init(&wxr->index);

	return wxr;

err_map:
err_stat:
	close(wxr->fd);
err_open:
	g_free((void*)wxr->file_name);
	g_free(wxr);
	return NULL;
}

void wxr_ctx_close(wxr_ctx *wxr)
{
	wxr_index_cleanup(&wxr->index);
	size_t map_size = ROUND_INT_UP_TO(wxr->file_size+1, PAGE_SIZE);
	munmap(wxr->map, map_size);
	close(wxr->fd);
	g_free((void*)wxr->file_name);
	g_free(wxr);
}

void wxr_ctx_get_contents(wxr_ctx *wxr,
			  const char **ptr, size_t *size)
{
	*ptr = wxr->map;
	*size = wxr->file_size;
}

bool wxr_ctx_build_index(wxr_ctx *wxr, GError **error)
{
	RETURN_IF(wxr->indexed, true);
	char *p = wxr->map;
	char *e = wxr->map + wxr->file_size;
	char *prev = NULL;
	wxr_date prev_date;
	bool success;

	while (p < e) {
		wxr_date d;
		int n=0, rc;

		rc = sscanf(p, "%hu-%hhu-%hhu\n%n",
			    &d.year, &d.month, &d.day, &n);
#if 0
		printf("rc=%d year=%d month=%d day=%d n=%d\n",
		       rc, d.year, d.month, d.day, n);
#endif

		if (rc == 3 && n>6) {
			/* found a date like thing */
			if (prev) {
				/* index everything upto this date */
				success = wxr_index_add(&wxr->index, prev_date,
							prev, p-prev, error);
				if (!success) {
					g_prefix_error(error, "%s:%zu: ",
						       wxr->file_name,
						       (void*)p - wxr->map);
					return false;
				}
			}

			/* skip this line */
			p += n;

			/* mark this point for the next iteration */
			prev_date = d;
			prev = p;

		} else {
			/* skip this line */
			while (p<e && *p != '\n' && *p != '\r') p++;
			while (p<e && (*p == '\n' || *p == '\r')) p++;
		}
	}

	if (prev && prev!=p) {
		/* file ended, but we have one last matched block */
		success = wxr_index_add(&wxr->index, prev_date,
					prev, p-prev, error);
		if (!success) {
			g_prefix_error(error, "%s:%zu: ",
				       wxr->file_name,
				       (void*)p - wxr->map);
			return false;
		}
	}

	wxr->indexed = true;
	return true;
}

size_t wxr_ctx_session_count(wxr_ctx *wxr)
{
	return wxr->index.count;
}

const wxr_index * wxr_ctx_get_index(wxr_ctx *wxr, GError **error)
{
	if (!wxr_ctx_build_index(wxr, error))
		return NULL;

	return &wxr->index;
}

long wxr_ctx_enumerate(wxr_ctx *wxr, wxr_enumerate_ent_cb_fn cb_fn,
		      void *opaque, GError **error)
{
	const wxr_index *index = wxr_ctx_get_index(wxr, error);
	if (!index)
		return -1;

	long count = 0, rc;
	wxr_index_for_each_session(index, i, ses) {
		wxr_session_for_each_lift(ses, j, lift) {
			wxr_lift_for_each_entry(lift, k, ent) {
				rc = cb_fn(wxr, ses,  lift, ent, opaque, error);
				if (rc<0)
					return rc;
				count ++;
			}
		}
	}

	return count;
}

long wxr_ctx_filter_enumerate(wxr_ctx *wxr,
			      wxr_filter_ses_cb_fn filter_ses,
			      wxr_filter_lift_cb_fn filter_lift,
			      wxr_filter_ent_cb_fn filter_ent,
			      wxr_enumerate_ent_cb_fn cb_fn,
			      void *opaque, GError **error)
{
	const wxr_index *index = wxr_ctx_get_index(wxr, error);
	if (!index)
		return -1;

	long count = 0, rc;
	wxr_index_for_each_session(index, i, ses) {
		rc = filter_ses(wxr, ses, opaque, error);
		if (rc<0) return rc;
		if (!rc) continue;

		wxr_session_for_each_lift(ses, j, lift) {
			rc = filter_lift(wxr, lift, opaque, error);
			if (rc<0) return rc;
			if (!rc) continue;

			wxr_lift_for_each_entry(lift, k, ent) {
				rc = filter_ent(wxr, ent, opaque, error);
				if (rc<0) return rc;
				if (!rc) continue;

				rc = cb_fn(wxr, ses,  lift, ent, opaque, error);
				if (rc<0)
					return rc;
				count ++;
			}
		}
	}

	return count;
}
