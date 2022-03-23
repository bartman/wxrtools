#include <glib.h>
#include <string.h>
#include <ctype.h>

#include "wxr.h"

#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"


/* allocates a single buffer that contains an array of strings pointers, and
 * the strings themselves. the entire structure is freed with g_free() after */
char ** g_malloc_strv(size_t count, size_t size)
{
	size_t array_size = (count+1) * sizeof(char*);
	size_t data_size = count * size;
	size_t total_size = array_size + data_size;

	/* too big, don't bother */
	if (count > INT_MAX || size > INT_MAX)
		return NULL;

	char **array = g_malloc(total_size);
	RETURN_IF(!array, NULL);

	char *p = (void*)array + array_size;

	for (size_t i=0; i<count; i++) {
		array[i] = p;
		p += size;
	}
	array[count] = NULL;

	return array;
}
