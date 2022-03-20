#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "wxr_lib_index.h"

#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"

void wxr_index_init(wxr_index *index)
{
	index->size = 0;
	index->count = 0;
	index->sessions = NULL;
}

void wxr_index_cleanup(wxr_index *index)
{
	g_free(index->sessions);
}

bool wxr_index_add(wxr_index *index, wxr_date date,
		   const char *text, size_t text_len, GError **error)
{
	if (index->count >= index->size) {
		/* need to grow */
		size_t new_size = index->size + 128;
		size_t total_size = new_size * sizeof(*index->sessions);
		gpointer new = g_realloc(index->sessions, total_size);
		if (!new) {
			wxr_set_error_errno(error, "realloc index (%zu)",
					    new_size);
			return false;
		}
		index->size = new_size;
		index->sessions = new;
	}

	wxr_session *s = index->sessions + (index->count++);

	s->date = date;
	s->text = text;
	s->text_len = text_len;

	return true;
}
