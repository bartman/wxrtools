#include <glib.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "wxr_lib_ctx.h"
#include "wxr_ctx.h"

#include "wxr_utils.h"
#include "wxr_error.h"

struct wxr_ctx* wxr_ctx_open(const char *file_name, GError **error)
{
	struct wxr_ctx *wxr;

	size_t total_size = sizeof(*wxr);
	wxr = g_malloc(total_size);
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

	size_t map_size = ROUND_INT_UP_TO(wxr->file_size, PAGE_SIZE);
	wxr->map = mmap(NULL, map_size, PROT_READ, MAP_SHARED,  wxr->fd, 0);
	if (wxr->map == MAP_FAILED) {
		wxr_set_error_errno(error, "mmap(%s)", file_name);
		goto err_map;
	}

	return wxr;

err_map:
err_stat:
	close(wxr->fd);
err_open:
	g_free((void*)wxr->file_name);
	g_free(wxr);
	return NULL;
}

void wxr_ctx_close(struct wxr_ctx *wxr)
{
	size_t map_size = ROUND_INT_UP_TO(wxr->file_size, PAGE_SIZE);
	munmap(wxr->map, map_size);
	close(wxr->fd);
	g_free((void*)wxr->file_name);
	g_free(wxr);
}

void wxr_ctx_get_contents(struct wxr_ctx *wxr,
			  const char **ptr, size_t *size)
{
	*ptr = wxr->map;
	*size = wxr->file_size;
}
