
#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>

/**
 * @def P_BIT
 * @brief Present bit
 *
 * 0x80, 0b10000000
 */
#define P_BIT    (1 << 7)
#define DPL_NONE 0

/**
 * @enum idt_gate_types
 * @brief Gate type numbers for the IDT.
 * @details The 2 main types of ISR (Interrupt Service Routines) are traps and
 * interrupts. Traps are exceptions generated by errors, and store the address
 * of the current instruction. Interrupts are used to handle events that need
 * attention unrelated to the current instruction.
 *
 * For more information, see: https://wiki.osdev.org/IDT#Gate_Types
 */
enum idt_gate_types {
    IDT_GATE_TASK       = 0x5, /**< @brief Task gate */
    IDT_GATE_16BIT_INT  = 0x6, /**< @brief 16 bit interrupt */
    IDT_GATE_16BIT_TRAP = 0x7, /**< @brief 16 bit trap */
    IDT_GATE_32BIT_INT  = 0xE, /**< @brief 32 bit interrupt. Used. */
    IDT_GATE_32BIT_TRAP = 0xF, /**< @brief 32 bit trap */
};

/**
 * @enum pic_ports
 * @brief IO ports for the master and slave PICs.
 * @details PIC means Programmable Interrupt Controller
 *
 * See https://wiki.osdev.org/PIC
 */
enum pic_ports {
    PIC_MASTER_CMD  = 0x20, /**< @brief Command I/O port of the master PIC */
    PIC_MASTER_DATA = 0x21, /**< @brief Data I/O port of the master PIC */
    PIC_SLAVE_CMD   = 0xA0, /**< @brief Command I/O port of the slave PIC */
    PIC_SLAVE_DATA  = 0xA1, /**< @brief Data I/O port of the slave PIC */
};

/**
 * @enum pic_flags
 * @brief Flags used for the data and command pic_ports
 */
enum pic_flags {
    ICW1_ICW4      = 0x01, /**< @brief ICW4 (not) needed */
    ICW1_SINGLE    = 0x02, /**< @brief Single (cascade) mode */
    ICW1_INTERVAL4 = 0x04, /**< @brief Call address interval 4 (8) */
    ICW1_LEVEL     = 0x08, /**< @brief Level triggered (edge) mode */
    ICW1_INIT      = 0x10, /**< @brief Initialization - required! */

    ICW4_8086       = 0x01, /**< @brief 8086/88 (MCS-80/85) mode */
    ICW4_AUTO       = 0x02, /**< @brief Auto (normal) EOI */
    ICW4_BUF_SLAVE  = 0x08, /**< @brief Buffered mode/slave */
    ICW4_BUF_MASTER = 0x0C, /**< @brief Buffered mode/master */
    ICW4_SFNM       = 0x10, /**< @brief Special fully nested (not) */

};

/**
 * @struct idt_entry
 * @brief Entry structure of the IDT
 *
 * @details IDT means Interrupt Descriptor Table. This table will contain
 * entries that give information about the ISRs (Interrupt Service Routines) of
 * each interrupt number, plus some other flags and information.
 *
 * See https://wiki.osdev.org/IDT
 */
typedef struct {
    uint16_t offset_l; /**< @brief Bits 0..15 of the offset. The offset is
                        * relative to the selected GDT segment. */
    uint16_t selector; /**< @brief Code segment selector of the GDT (or LDT) */
    uint8_t zero;      /**< @brief Unused, 0 */
    uint8_t type; /**< @brief Gate type, DPL (permissions), "Present" field */
    uint16_t offset_h; /**< @brief Bits 16..31 of the offset */
} __attribute__((packed)) idt_entry;

/**
 * @struct __attribute__
 * @brief Actual idt descriptor structure needed by the `lidt` assembly
 * instruction.
 * @details Same as the gdt descriptor from src/kernel/gdt.asm
 * */
typedef struct {
    uint16_t limit;  /**< @brief Size of the IDT minus 1 */
    idt_entry* base; /**< @brief Address of the IDT itself */
} __attribute__((packed)) idt_descriptor;

/**
 * @brief Initialize the idt and the idt descriptor
 * @details Defined in src/kernel/idt.c
 */
void idt_init(void);

/**
 * @brief Loads the idt descriptor from assembly using the lidt instruction.
 * @details Defined in src/kernel/idt.asm
 * @param[in] idt_desc Pointer to the idt_descriptor structure
 */
void idt_load(void* idt_desc);

#endif /* _KERNEL_IDT_H */
