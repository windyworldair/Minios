/*
 * File: linker.ld
 * Purpose: Linker script for the MiniOS64 x86_64 freestanding kernel.
 * Author: zalcus, 2026
 */

ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 1M;

    .multiboot ALIGN(8) :
    {
        KEEP(*(.multiboot))
    } :text

    .text ALIGN(4K) :
    {
        *(.text*)
    } :text

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    } :text

    .data ALIGN(4K) :
    {
        *(.data*)
    } :data

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss*)
    } :data
}
