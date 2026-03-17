/*
 * File: boot.s
 * Purpose: Multiboot2 entry, paging setup, and transition to x86_64 long mode.
 * Author: zalcus, 2026
 */

.set MB2_MAGIC,      0xE85250D6
.set MB2_ARCH,       0
.set MB2_HEADER_LEN, mb2_header_end - mb2_header_start
.set MB2_CHECKSUM,   -(MB2_MAGIC + MB2_ARCH + MB2_HEADER_LEN)

.section .multiboot
.align 8
mb2_header_start:
.long MB2_MAGIC
.long MB2_ARCH
.long MB2_HEADER_LEN
.long MB2_CHECKSUM

.short 0
.short 0
.long 8
mb2_header_end:

.section .bss
.align 16
mbi_ptr:
.quad 0

.align 16
stack_bottom:
.skip 32768
stack_top:

.align 4096
pml4_table:
.skip 4096

.align 4096
pdpt_table:
.skip 4096

.align 4096
pd_table:
.skip 4096

.section .rodata
.align 8
gdt64:
.quad 0x0000000000000000
.quad 0x00AF9A000000FFFF
.quad 0x00AF92000000FFFF
gdt64_end:

gdt64_ptr:
.word gdt64_end - gdt64 - 1
.quad gdt64

.section .text
.code32
.global _start
.extern kernel_main

_start:
    cli
    mov $stack_top, %esp
    mov %ebx, mbi_ptr

    call setup_page_tables
    call enable_long_mode

    lgdt gdt64_ptr
    ljmp $0x08, $long_mode_start

setup_page_tables:
    movl $pdpt_table, %eax
    orl $0x3, %eax
    movl %eax, pml4_table

    movl $pd_table, %eax
    orl $0x3, %eax
    movl %eax, pdpt_table

    movl $pd_table, %edi
    xorl %ecx, %ecx
1:
    movl %ecx, %eax
    shll $21, %eax
    orl $0x83, %eax
    movl %eax, (%edi)
    movl $0x0, 4(%edi)
    addl $8, %edi
    incl %ecx
    cmpl $512, %ecx
    jne 1b
    ret

enable_long_mode:
    movl $pml4_table, %eax
    movl %eax, %cr3

    movl %cr4, %eax
    orl $0x20, %eax
    movl %eax, %cr4

    movl $0xC0000080, %ecx
    rdmsr
    orl $0x100, %eax
    wrmsr

    movl %cr0, %eax
    orl $0x80000001, %eax
    movl %eax, %cr0
    ret

.code64
long_mode_start:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    mov mbi_ptr(%rip), %rdi
    call kernel_main

halt:
    cli
    hlt
    jmp halt

.section .note.GNU-stack,"",@progbits
