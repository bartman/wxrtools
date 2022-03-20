#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "wxr_lib_entry.h"

#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"

bool wxr_entry_init(wxr_entry *ent, float weight, unsigned reps, unsigned sets,
		    GError **error)
{
	memset(ent, 0, sizeof(*ent));
	ent->weight = weight;
	ent->reps = reps;
	ent->sets = sets;

	printf("%f x %u x %u\n",
	       weight, reps, sets);

	return true;
}

void wxr_entry_cleanup(wxr_entry *ent)
{
}
