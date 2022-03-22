/* vim: set noet sw=8 ts=8 tw=80: */
#pragma once

#include <stdio.h>

#include "wxr_types.h"

extern float wxr_entry_1rm(const wxr_entry *ent);

extern float wxr_entry_total_volume(const wxr_entry *ent);

extern int wxr_entry_fprintf(FILE *out, const wxr_entry *ent);
