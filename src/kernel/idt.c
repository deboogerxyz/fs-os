
/*
 * Interrupt Descriptor Table. For more information, see:
 *   https://wiki.osdev.org/Interrupt_Descriptor_Table
 *   http://www.brokenthorn.com/Resources/OSDevPic.html
 */

#include <stdint.h>
#include <stdlib.h>
#include <kernel/io.h>
#include <kernel/idt.h>
#include <kernel/exceptions.h>

#define IDT_SZ 256

/** @brief Interrupt descriptor table itself, 256 entries. */
idt_entry idt[IDT_SZ] = { 0 };

/** @brief The descriptor containing the idt size and ptr. Initialized in
 * idt_init. */
idt_descriptor descriptor;

/**
 * @brief Registers an interrupt service routine in the selected index of the
 * idt array.
 * @param idx Index of the idt array.
 * @param func Casted pointer to ISR function.
 */
static void register_isr(uint16_t idx, uint32_t func) {
    if (idx >= IDT_SZ)
        panic_line("Idx out of bounds when registering ISR.");

    idt[idx] = (idt_entry){
        .selector = 0x8, /* 00000000 00001000. Last 3 bits of the selector are
                            TI and RPL. We only want to set the idx to 1 (first
                            idx is the null gdt entry) */
        .offset_l = ((uint32_t)func) & 0xFFFF,
        .offset_h = (((uint32_t)func) >> 16) & 0xFFFF,
        .type     = P_BIT | DPL_NONE | IDT_GATE_32BIT_INT,
        .zero     = 0,
    };
}

/**
 * @brief Remap the programmable interrupt controllers so the interrupt numbers
 * of the master PIC don't overlap with the CPU exceptions.
 *
 * See wiki for more details.
 */
static inline void pic_remap(void) {
    /* Start the initialization sequence in cascade mode */
    io_outb(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
    io_outb(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);

    /* Make the master PIC start in the interrupt number 32 instead of 8
     * Make the slave PIC start in the interrupt number 40 instead of 0x70 */
    io_outb(PIC_MASTER_DATA, 32);
    io_outb(PIC_SLAVE_DATA, 40);

    /* Slave PIC is chained to the master */
    io_outb(PIC_MASTER_DATA, 4);
    io_outb(PIC_SLAVE_DATA, 2);

    /* We are in 32 bits */
    io_outb(PIC_MASTER_DATA, ICW4_8086);
    io_outb(PIC_SLAVE_DATA, ICW4_8086);

    /* We don't need to restore saved masks because they are not masked */
    io_outb(PIC_MASTER_DATA, 0);
    io_outb(PIC_SLAVE_DATA, 0);
}

void idt_init(void) {
    /* Descriptor */
    descriptor.limit = (IDT_SZ * sizeof(idt_entry)) - 1;
    descriptor.base  = &idt[0];

    /* Remap the PICs so the interrupt numbers of the master PIC don't overlap
     * with the CPU exceptions. See comment inside function. */
    pic_remap();

    /* Exception Handling. exc_* defined in src/kernel/idt.asm */
    register_isr(0, (uint32_t)&exc_0);
    register_isr(1, (uint32_t)&exc_1);
    register_isr(2, (uint32_t)&exc_2);
    register_isr(3, (uint32_t)&exc_3);
    register_isr(4, (uint32_t)&exc_4);
    register_isr(5, (uint32_t)&exc_5);
    register_isr(6, (uint32_t)&exc_6);
    register_isr(7, (uint32_t)&exc_7);
    register_isr(8, (uint32_t)&exc_8);
    register_isr(10, (uint32_t)&exc_10);
    register_isr(11, (uint32_t)&exc_11);
    register_isr(12, (uint32_t)&exc_12);
    register_isr(13, (uint32_t)&exc_13);
    register_isr(14, (uint32_t)&exc_14);
    register_isr(15, (uint32_t)&exc_15);
    register_isr(16, (uint32_t)&exc_16);
    register_isr(17, (uint32_t)&exc_17);
    register_isr(18, (uint32_t)&exc_18);
    register_isr(19, (uint32_t)&exc_19);
    register_isr(20, (uint32_t)&exc_20);
    register_isr(30, (uint32_t)&exc_30);

    /* IRQs. See src/kernel/idt.asm */
    register_isr(32, (uint32_t)&irq_pit); /* PIT. IRQ 0 */
    register_isr(33, (uint32_t)&irq_kb);  /* Keyboard. IRQ 1 */

    /* Unused IRQs, just ignore. See src/kernel/idt.asm */
    for (int i = 34; i < 40; i++)
        register_isr(i, (uint32_t)&irq_default_master);
    for (int i = 40; i < 48; i++)
        register_isr(i, (uint32_t)&irq_default_slave);

    /* See src/kernel/idt.asm */
    idt_load(&descriptor);

    /* Enable interrupts (opposite of cli) */
    asm("sti");
}
