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
	if (ent->weight == 0)
		return 0;
	if (ent->reps == 1)
		return ent->weight;

	/* https://en.wikipedia.org/wiki/One-repetition_maximum */

#if defined(ONE_REP_MAX_EPLEY)
	return ent->weight * (1.0 + (ent->reps / 30.0));
#elif defined(ONE_REP_MAX_BRZYCKI)
	return ent->weight * (36.0 / (37.0 - ent->reps));
#elif defined(ONE_REP_MAX_LOMBARDI)
	return ent->weight * pow(ent->reps, 0.1);
#elif defined(ONE_REP_MAX_MAYHEW)
	return (100.0 * ent->weight) / (52.2 + 41.9 * exp(-0.055 * ent->reps));
#elif defined(ONE_REP_MAX_OCONNER)
	return ent->weight * (1.0 + (ent->reps / 40.0));
#elif defined(ONE_REP_MAX_WATHEN)
	return (100.0 * ent->weight)  / (48.80 + 53.8 * exp(-0.75 * ent->reps));
#else
#error(ONE_REP_MAX_ not specified)
#endif
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
