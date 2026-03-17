/*
 * File: memory.h
 * Purpose: Minimal physical frame allocator for early kernel memory management.
 * Author: zalcus, 2026
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>

void memory_init(void);
bool memory_alloc_frame(uint64_t *out_addr);
void memory_free_frame(uint64_t addr);
uint32_t memory_total_frames(void);
uint32_t memory_used_frames(void);

#endif /* MEMORY_H */
