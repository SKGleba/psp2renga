/* 
	NMFramework by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/
#include "types.h"

// Copy the framework and hook sm_load & uirq_handle
void __attribute__((optimize("O0"))) _start(void) {
	u32_t ret = 0;
	u32_t (*pacpy)(int dst, int src, int sz) = (void*)((u32_t)0x00801016);
	ret = pacpy((u32_t)0x00807c50, (u32_t)0x1C1FD000, 0x50);
	ret = pacpy((u32_t)0x00809e00, (u32_t)0x1C1FB000, 0x80);
	*(u16_t *)0x00800a06 = 0xd050;
	*(u16_t *)0x00800a08 = 0x807c;
	*(u16_t *)0x00800a0a = 0x100f;
	*(u16_t *)0x00800326 = 0xd000;
	*(u16_t *)0x00800328 = 0x809e;
	return;
}
