/* 
	psp2renga by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

static volatile u32_t * const BIGMAC = (void *)0xE0050000;

u32_t _start(u32_t cfg_paddr, unsigned int fcmd) {
	u32_t ret = 0;
	NMFfm_nfo *fmnfo = (void *)cfg_paddr;
	u32_t udst = *(u32_t *)((u32_t)fmnfo->cucnfo.unused);
	u32_t usrc = *(u32_t *)((u32_t)fmnfo->cucnfo.unused + 4);
	u32_t usz = *(u32_t *)((u32_t)fmnfo->cucnfo.unused + 8);
	u32_t device = *(u32_t *)((u32_t)fmnfo->cucnfo.unused + 12);
	if (device == 0) {
		u32_t (*pacpy)(int dst, int src, int sz) = (void*)((u32_t)0x00801016);
		ret = pacpy(udst, usrc, usz);
	} else {
		if (BIGMAC[9] & 1) {
			BIGMAC[7] = 0;
			while (BIGMAC[9] & 1) {}
		}
		BIGMAC[0] = usrc;
		BIGMAC[1] = udst;
		BIGMAC[2] = usz;
		BIGMAC[3] = 0x2080 & 0xfffffff8;
		BIGMAC[7] = 1;
		while (BIGMAC[9] & 1) {}
		ret = BIGMAC[9];
	}
	return ret;
}
