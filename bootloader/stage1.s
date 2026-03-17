/*
 * File: stage1.s
 * Purpose: BIOS boot sector to load MiniOS stage2 from floppy sectors.
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
    movw $0x7C00, %sp
    sti

    movb %dl, boot_drive

    movw $banner, %si
print_banner:
    lodsb
    testb %al, %al
    jz load_stage2
    movb $0x0E, %ah
    movw $0x0007, %bx
    int $0x10
    jmp print_banner

load_stage2:
    movb $0x00, %ah
    movb boot_drive, %dl
    int $0x13
    jc disk_error

    movw $0x1000, %ax
    movw %ax, %es
    xorw %bx, %bx

    movb $0x02, %ah
    movb $0x08, %al
    movb $0x00, %ch
    movb $0x00, %dh
    movb $0x02, %cl
    movb boot_drive, %dl
    int $0x13
    jc disk_error

    ljmp $0x1000, $0x0000

disk_error:
    movw $disk_fail, %si
print_error:
    lodsb
    testb %al, %al
    jz halt
    movb $0x0E, %ah
    movw $0x0004, %bx
    int $0x10
    jmp print_error

halt:
    cli
    hlt
    jmp halt

banner:
    .asciz "MiniOS bootloader v1\r\n"
disk_fail:
    .asciz "Disk read error\r\n"

boot_drive:
    .byte 0

.org 510
.word 0xAA55
