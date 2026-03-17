/*
 * File: interrupts.c
 * Purpose: Minimal interrupt architecture foundation (IDT/PIC/PIT).
 * Author: zalcus, 2026
 */

#include "interrupts.h"

#include "io.h"

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;

static idt_entry_t idt[256];

__attribute__((naked)) static void default_isr(void)
{
    __asm__ volatile ("iretq");
}

static void idt_set_gate(uint8_t num, uint64_t handler)
{
    idt[num].offset_low = (uint16_t)(handler & 0xFFFFU);
    idt[num].selector = 0x08U;
    idt[num].ist = 0U;
    idt[num].type_attr = 0x8EU;
    idt[num].offset_mid = (uint16_t)((handler >> 16U) & 0xFFFFU);
    idt[num].offset_high = (uint32_t)((handler >> 32U) & 0xFFFFFFFFU);
    idt[num].zero = 0U;
}

void idt_init(void)
{
    const uint64_t h = (uint64_t)(uintptr_t)default_isr;
    for (uint32_t i = 0; i < 256U; ++i) {
        idt_set_gate((uint8_t)i, h);
    }

    const idt_ptr_t ptr = {
        .limit = (uint16_t)(sizeof(idt) - 1U),
        .base = (uint64_t)(uintptr_t)&idt[0],
    };

    __asm__ volatile ("lidt %0" : : "m"(ptr));
}

void pic_remap(uint8_t offset1, uint8_t offset2)
{
    const uint8_t a1 = inb(0x21);
    const uint8_t a2 = inb(0xA1);

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, offset1);
    outb(0xA1, offset2);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, a1);
    outb(0xA1, a2);
}

void pit_init(uint32_t frequency_hz)
{
    if (frequency_hz == 0U) {
        frequency_hz = 100U;
    }
    uint32_t divisor = 1193182U / frequency_hz;
    if (divisor == 0U) {
        divisor = 1U;
    }

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFFU));
    outb(0x40, (uint8_t)((divisor >> 8U) & 0xFFU));
}
