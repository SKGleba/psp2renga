/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

/*
	"BIG" function
	TODO: document
*/
u8_t _start(void) {
	NMFfm_nfo *fmnfo = (void *)*(u32_t *)0x00809e80;
	u8_t ret = 0x11, maxetr = RENGA_ENTRIES_MAX, curetr = 0, found = 0;
	u32_t tmp = 0;
	if (fmnfo->status == 0x34) {
		fmnfo->status = 0x12;
		while (curetr < maxetr) {
			if (fmnfo->entries[curetr].magic == 0) {
				maxetr = 0;
				break;
			} else if (fmnfo->entries[curetr].magic == *(u16_t *)0x0080b00e && fmnfo->entries[curetr].do_use > 0) {
				maxetr = 0;
				found = 1;
				break;
			} else
				curetr = curetr + 1;
		}
		fmnfo->status = 0x13;
		if (found == 1) {
			fmnfo->entries[curetr].status = 0x34;
			if ((fmnfo->entries[curetr].do_use & 0xf0) == RENGA_TYPE_SM) {
				u32_t (*pacpy)(int dst, int src, int sz) = (void*)((u32_t)0x00801016);
				tmp = pacpy((u32_t)0x0080b000, (u32_t)(*(u32_t *)0x00809e80 + RENGA_BLOCKS_START + (curetr * RENGA_BLOCK_SZ)), (u32_t)fmnfo->entries[curetr].smsz);
				fmnfo->entries[curetr].status = 0x69;
			}
			if ((fmnfo->entries[curetr].do_use & 0x0f) == RENGA_TYPE_CODE) {
				u8_t (*sm_cuc)() = (void*)((u32_t)(*(u32_t *)0x00809e80 + RENGA_BLOCKS_START + (curetr * RENGA_BLOCK_SZ) + RENGA_MAX_SM_SZ));
				fmnfo->entries[curetr].status = sm_cuc();
			}
		}
		fmnfo->status = 0x14;
		ret = 0x34;
	}
	return ret;
}
