#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "wxr.h"

#include "wxr_config.h"
#include "wxr_types.h"
#include "wxr_utils.h"
#include "wxr_error.h"


double wxr_1rm_from_weight_reps(double weight, unsigned reps)
{
	if (weight<=0 || !reps)
		return 0;
	if (reps == 1)
		return weight;

	/* https://en.wikipedia.org/wiki/One-repetition_maximum */

#if defined(ONE_REP_MAX_EPLEY)
	return weight * (1.0 + (reps / 30.0));
#elif defined(ONE_REP_MAX_BRZYCKI)
	return weight * (36.0 / (37.0 - reps));
#elif defined(ONE_REP_MAX_LOMBARDI)
	return weight * pow(reps, 0.1);
#elif defined(ONE_REP_MAX_MAYHEW)
	return (100.0 * weight) / (52.2 + 41.9 * exp(-0.055 * reps));
#elif defined(ONE_REP_MAX_OCONNER)
	return weight * (1.0 + (reps / 40.0));
#elif defined(ONE_REP_MAX_WATHEN)
	return (100.0 * weight)  / (48.80 + 53.8 * exp(-0.75 * reps));
#else
#error(ONE_REP_MAX_ not specified)
#endif
}

double wxr_weight_from_1rm_reps(double onerm, unsigned reps)
{
	if (onerm<=0 || !reps)
		return 0;
	if (reps == 1)
		return onerm;

#if defined(ONE_REP_MAX_EPLEY)
	return onerm / (1.0 + (reps / 30.0));
#elif defined(ONE_REP_MAX_BRZYCKI)
	return onerm / (36.0 / (37.0 - reps));
#elif defined(ONE_REP_MAX_LOMBARDI)
	return onerm / pow(reps, 0.1);
#elif defined(ONE_REP_MAX_MAYHEW)
	return (onerm / 100.0) * (52.2 + 41.9 * exp(-0.055 * reps));
#elif defined(ONE_REP_MAX_OCONNER)
	return onerm / (1.0 + (reps / 40.0));
#elif defined(ONE_REP_MAX_WATHEN)
	return (onerm / 100.0) * (48.80 + 53.8 * exp(-0.75 * reps));
#else
#error(ONE_REP_MAX_ not specified)
#endif
}

unsigned wxr_reps_from_1rm_weight(double onerm, double weight)
{
	if (onerm<=0 || weight<=0 || onerm<weight)
		return 0;
	if (onerm == weight)
		return 1;

	double reps;
#if defined(ONE_REP_MAX_EPLEY)
	reps = 30.0 * ((onerm/weight) - 1);
#elif defined(ONE_REP_MAX_BRZYCKI)
	reps = 37.0 - (36.0 / (onerm/weight));
#elif defined(ONE_REP_MAX_LOMBARDI)
	reps = pow(onerm/weight, 10);
#elif defined(ONE_REP_MAX_MAYHEW)
	onerm = (100.0 * weight) / (52.2 + 41.9 * exp(-0.055 * reps));
#elif defined(ONE_REP_MAX_OCONNER)
	reps = 40.0 * ((onerm/weight) - 1);
#elif defined(ONE_REP_MAX_WATHEN)
	onerm = (100.0 * weight)  / (48.80 + 53.8 * exp(-0.75 * reps));
#else
#error(ONE_REP_MAX_ not specified)
#endif
	return floor(reps);
}
