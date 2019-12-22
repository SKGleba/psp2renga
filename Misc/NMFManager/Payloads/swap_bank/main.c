/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

// Copy the framework and hook sm_load & uirq_handle
u32_t __attribute__((optimize("O0"))) _start(u32_t paddr) {
	NMFfm_nfo *fmnfo = (void *)*(u32_t *)0x00809e80;
	*(u32_t *)0x00809e80 = (u32_t)fmnfo->cucnfo.unused;
	return 0x6934;
}