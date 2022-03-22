/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include "wxr_config.h"
#include "wxr_types.h"

extern double wxr_1rm_from_weight_reps(double weight, unsigned reps);
extern double wxr_weight_from_1rm_reps(double onerm, unsigned reps);
extern unsigned wxr_reps_from_1rm_weight(double onerm, double weight);
