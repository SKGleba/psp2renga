/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "../../../../Include/renga-defs.h"

// Copy the framework and hook sm_load & fcmd_handle
u32_t __attribute__((optimize("O0"))) _start(void) {
	u32_t ret = 0;
	u32_t (*pacpy)(int dst, int src, int sz) = (void*)((u32_t)0x00801016); // memcpy func
	ret = pacpy((u32_t)0x00807c50, (u32_t)0x1f850400, 0x50); // copy sm_load payload
	ret = pacpy((u32_t)0x00809e00, (u32_t)0x1f850800, 0x80); // copy fcmd_handle payload
	*(u32_t *)0x00809e80 = *(u32_t *)0x1f850B00; // copy framework cfg paddr
	if (*(u32_t *)0x00809e80 > 0) { // make sure that cfg paddr is not 0 before hooking
		
		// sm_load::set_state(5) -> jsr (0x00807c50)
		*(u16_t *)0x00800a06 = 0xd050;
		*(u16_t *)0x00800a08 = 0x807c;
		*(u16_t *)0x00800a0a = 0x100f;
		
		// fcmd_handler() -> bsr (0x00809e00)
		*(u16_t *)0x00800372 = 0xdc79;
		*(u16_t *)0x00800374 = (u16_t)0x009a;
		
		// fcmd_handler::get_cmd -> mov r6,r1
		*(u16_t *)0x00800afe = 0x0610;
	}
	ret = *(u32_t *)0x00809e80;
	return ret;
}
