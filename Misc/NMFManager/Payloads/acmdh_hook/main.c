/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

// Execute code if requested or resume the CMDhandler
void _start(void) {
	unsigned int acmd = *(unsigned int *)0xe0000010; // get ARM cmd
	NMFfm_nfo *fmnfo = (void *)*(u32_t *)0x00809e80; // framework cfg
	if (fmnfo->cucnfo.paddr > 0) {
		u32_t (*ccode)(u32_t cfg_paddr, unsigned int fcmd) = (void*)((u32_t)fmnfo->cucnfo.paddr);
		fmnfo->cucnfo.resp = ccode(*(u32_t *)0x00809e80, acmd); // run user code, store ret in fmnfo->cucnfo.resp
		fmnfo->cucnfo.paddr = 0; // make sure to not run more than once
	} else {
		void (*fcmd_handler)(unsigned int fcmd) = (void*)((u32_t)0x00800adc);
		fcmd_handler(acmd); // run cmd handler as usual
	}
	return;
}