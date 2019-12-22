/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

// Check manager magic and jump to it
void _start(void) {
	NMFfm_nfo *fmnfo = (void *)*(u32_t *)0x00809e80;
	if (fmnfo->do_use == 1) {
		u8_t (*sm_cmgr)() = (void*)((u32_t)fmnfo->mgrpaddr);
		fmnfo->status = sm_cmgr();
	}
	void (*set_status)() = (void*)((u32_t)0x00802010);
	set_status(5);
	return;
}
