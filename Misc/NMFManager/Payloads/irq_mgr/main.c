/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "types.h"
typedef struct {
  u32_t magic;
  u32_t paddr;
  u32_t resp;
} __attribute__((packed)) cuc_nfo;
static volatile cuc_nfo * const cucnfo = (void *)0x1C0D00F0;

// Execute code if requested and then resume IRQ
void _start(int irq) {
	if (cucnfo->magic == 0xBEEFCAFE && cucnfo->paddr > 0) {
		u32_t (*ccode)(int cur_irq) = (void*)((u32_t)cucnfo->paddr);
		cucnfo->resp = ccode(irq);
		cucnfo->paddr = 0;
	}
	u32_t (*get_irq_off)(int entry) = (void*)((u32_t)0x008028d4);
	u32_t realoff = get_irq_off(irq);
	u32_t (*irq_h)(int irqn) = (void*)((u32_t)realoff);
	irq_h(irq);
	return;
}