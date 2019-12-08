/* 
	NMPRunner by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/

/*
	Stage 2 payload for Not-Moth:
	 - execute "big" code @0x1C010100
	 - clean r0
	 - jmp back to update_sm's 0xd0002
	Tested on 3.71 - 3.73
*/

void _start(void) {
	__asm__ (
		"movh $0, 0x1C01\n"
		"or3 $0, $0, 0x100\n"
		"jsr $0\n"
		"movh $0, 0x0\n"
		"movu $3, 0x80bd8c\n"
		"jmp $3\n"
	);
}
