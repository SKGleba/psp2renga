/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "types.h"

typedef struct {
  u16_t magic;
  u8_t do_use;
  u8_t status;
  u32_t mgrpaddr;
} __attribute__((packed)) NMFfm_nfo_small;

// Check manager magic and jump to it
void _start(void) {
	NMFfm_nfo_small *fmnfo = (void *)0x1C000000;
	if (fmnfo->magic == 0x6934 && fmnfo->do_use == 1) {
		fmnfo->status = 0x34;
		u8_t (*sm_cmgr)() = (void*)((u32_t)fmnfo->mgrpaddr);
		fmnfo->status = sm_cmgr();
	}
	void (*set_status)() = (void*)((u32_t)0x00802010);
	set_status(5);
	return;
}
