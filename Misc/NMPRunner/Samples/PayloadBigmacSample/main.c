/*
	bigmac comm sample (payload) for NMPRunner
	This code AES256-CBC bigmac-decrypts 0x200 bytes of
	data @0x1C000180 with key @0x1C012000 and IV @0x1C0120B0
	and writes results to 0x1C000700
*/

#include "types.h"
static volatile u32_t * const BIGMAC = (void *)0xE0050000;
static volatile u32_t * const RESULTS = (void *)0x1C000700;
void __attribute__((optimize("O0"))) _start(void) {
	
	// reset bigmac if running
	if (BIGMAC[9] & 1) {
		BIGMAC[7] = 0;
		while (BIGMAC[9] & 1) {}
	}
	
	// bigmac-memcpy the key from 0x1C012000 to 0xE0050200
	BIGMAC[0] = 0x1C012000;
	BIGMAC[1] = 0xE0050200;
	BIGMAC[2] = 0x20;
	BIGMAC[3] = 0x2080 & 0xfffffff8;
	BIGMAC[7] = 1;
	while (BIGMAC[9] & 1) {}
	RESULTS[0] = BIGMAC[9];
	
	// ARS256-CBC decrypt 0x200 bytes of data @0x1C000180 with IV @0x1C0120B0
	BIGMAC[0] = 0x1C000180;
	BIGMAC[1] = 0x1C000180;
	BIGMAC[2] = 0x200;
	BIGMAC[3] = (1 & 7) << 3 | 0x2080 & 0xfffffcc0 | 2 & 7 | (3 & 3) << 8;
	BIGMAC[4] = 0;
	BIGMAC[5] = 0x1C0120B0;
	BIGMAC[7] = 1;
	while (BIGMAC[9] & 1) {}
	RESULTS[1] = BIGMAC[9];
	return;
}