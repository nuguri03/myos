#ifndef PIC_H
#define PIC_H

#include "types.h"

void pic_remap(u8 offset1, u8 offset2);
void pic_send_eoi(u8 irq);

#endif