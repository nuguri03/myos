#ifndef PIT_H
#define PIT_H

#include "types.h"

void init_pit(u32 frequency);
u32 pit_get_ticks();

#endif