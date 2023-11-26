#ifndef _KERNEL_H
#define _KERNEL_H 1

#define PIC1 0x20
#define PIC2 0xA0		
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

#define PIC_EOI	0x20

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10
 
#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10	

#include <sys/cdefs.h>
#include <stdint.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

void enable_paging();
void sendEOI(unsigned char irq);
void initPIC(int offset1, int offset2);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);

#ifdef __cplusplus
}
#endif

#endif