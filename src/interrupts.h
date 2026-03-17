/*
 * File: interrupts.h
 * Purpose: IDT/PIC/PIT initialization interfaces for MiniOS64.
 * Author: zalcus, 2026
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void idt_init(void);
void pic_remap(uint8_t offset1, uint8_t offset2);
void pit_init(uint32_t frequency_hz);

#endif /* INTERRUPTS_H */
