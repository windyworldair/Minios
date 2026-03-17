/*
 * File: stage2.s
 * Purpose: Stage2 loader with A20, payload read, protected mode, and payload handoff.
 * Author: zalcus, 2026
 */

.code16
.section .text
.global _start

_start:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x9000, %sp

    movb %dl, boot_drive

    movw $msg_stage2, %si
    call print_string

    call enable_a20
    movw $msg_a20, %si
    call print_string

    call load_payload

    lgdt gdt_descriptor
    movl %cr0, %eax
    orl $0x1, %eax
    movl %eax, %cr0

    ljmp $0x08, $protected_mode_entry

print_string:
.next:
    lodsb
    testb %al, %al
    jz .done
    movb $0x0E, %ah
    movw $0x0007, %bx
    int $0x10
    jmp .next
.done:
    ret

enable_a20:
    inb $0x92, %al
    orb $0x02, %al
    outb %al, $0x92
    ret

load_payload:
    movb $0x00, %ah
    movb boot_drive, %dl
    int $0x13
    jc disk_error

    movw $0x8000, %ax
    movw %ax, %es
    xorw %bx, %bx

    movb $0x02, %ah
    movb $0x08, %al
    movb $0x01, %ch
    movb $0x01, %dh
    movb $0x0B, %cl
    movb boot_drive, %dl
    int $0x13
    jc disk_error
    ret

disk_error:
    movw $msg_disk, %si
    call print_string
.halt:
    cli
    hlt
    jmp .halt

.align 8
gdt_table:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF

gdt_descriptor:
    .word (gdt_table_end - gdt_table - 1)
    .long gdt_table
gdt_table_end:

.code32
protected_mode_entry:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    movl $msg_pm, %esi
    movl $0xB8000, %edi
    movb $0x1F, %ah
.pm_loop:
    lodsb
    testb %al, %al
    jz .jump_payload
    movw %ax, (%edi)
    addl $2, %edi
    jmp .pm_loop

.jump_payload:
    movl $0x00080000, %eax
    jmp *%eax

.section .rodata
msg_stage2:
    .asciz "MiniOS stage2 loaded.\r\n"
msg_a20:
    .asciz "A20 enabled, loading payload...\r\n"
msg_disk:
    .asciz "Payload disk read error\r\n"
msg_pm:
    .asciz "Stage2: Protected mode, jumping payload..."

.section .bss
boot_drive:
    .byte 0
