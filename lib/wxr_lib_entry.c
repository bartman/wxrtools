#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "wxr.h"

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

#if 0
	printf("%f x %u x %u\n",
	       weight, reps, sets);
#endif

	return true;
}

void wxr_entry_cleanup(wxr_entry *ent)
{
}

float wxr_entry_1rm(const wxr_entry *ent)
{
	return wxr_1rm_from_weight_reps(ent->weight, ent->reps);
}

float wxr_entry_total_volume(const wxr_entry *ent)
{
	return ent->weight * ent->reps * ent->sets;
}

int wxr_entry_fprintf(FILE *out, const wxr_entry *ent)
{
	return fprintf(out, "Entry { %.1f x %u x %u }",
		       ent->weight, ent->reps, ent->sets);
}
