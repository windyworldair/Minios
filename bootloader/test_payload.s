/*
 * File: test_payload.s
 * Purpose: Minimal 32-bit test payload for BIOS stage2 handoff validation.
 * Author: zalcus, 2026
 */

.code32
.section .text
.global _start

_start:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movl $0xB8000, %edi
    movl $msg, %esi
    movb $0x2E, %ah
.loop:
    lodsb
    testb %al, %al
    jz .halt
    movw %ax, (%edi)
    addl $2, %edi
    jmp .loop

.halt:
    cli
.spin:
    hlt
    jmp .spin

msg:
    .asciz "MiniOS BIOS test payload OK (VirtualBox test image)"
