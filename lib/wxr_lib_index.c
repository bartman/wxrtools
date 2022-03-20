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
}

void wxr_index_cleanup(wxr_index *index)
{
}

bool wxr_index_add(wxr_index *index, wxr_date date,
		   const char *str, size_t size, GError **error)
{
	static int num = 0;

	printf(" --- %u ---\n", num);
	printf("%04u-%02u-%02u\n", date.year, date.month, date.day);

	write(1, str, size);

	num ++;
	if (num>3)
		exit(1);

	return true;
}
