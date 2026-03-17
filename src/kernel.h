/*
 * File: kernel.h
 * Purpose: Main kernel interface and startup contract for MiniOS64.
 * Author: zalcus, 2026
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void kernel_main(uint64_t multiboot_info_addr);

#endif /* KERNEL_H */
