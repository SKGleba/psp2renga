/* 
	dsll_mdr72 by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/

#include <stdio.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include "debugScreen.h"
#include "../../../../Include/renga-defs.h"
#include "../../../../Include/renga_user-funcs.h"

#define printf(...) psvDebugScreenPrintf(__VA_ARGS__)

#define DECSM_PATH "ux0:data/update_sm.bin"
#define KLOG_PATH "ux0:data/0psp2renga.log"
#define RENGA_DUMP_PATH "ux0:data/dsll_framework_dump.bin"
#define BLOCKS_DUMP_PATH "ux0:data/dsll_blocks_dump.bin"
#define UNUSED_DUMP_PATH "ux0:data/dsll_unused_commem_dump.bin"

static void wait_key_press();
static int do_dump = 0;

int main(int argc, char *argv[])
{
	uint8_t ret8;

	psvDebugScreenInit();

	printf("dsll_mdr72 by SKGleba\n");

	renga_set_logging_for_user(1);
	
	ret8 = renga_get_status_for_user(RENGA_MAGIC_MASTER);
	printf("getting framework status... 0x%02X\n", ret8);
	if (ret8 == 0x22)
		renga_force_reset_framework_for_user(1);
	
	printf("adding entry %s... \n", DECSM_PATH);
	printf("\n main_status: 0x%02X\n\n", renga_add_entry_for_user(DECSM_PATH, 0x10, RENGA_MAGIC_373UPDATE));
	
	printf(" kernel log: %s\n\n", KLOG_PATH);
	
	wait_key_press();
	
	if (do_dump == 1) {
		printf("\ndumping commem...\n\n");
		renga_work_commem_for_user(RENGA_DUMP_PATH, 0, RENGA_BLOCKS_START, 0);
		renga_work_commem_for_user(BLOCKS_DUMP_PATH, RENGA_BLOCKS_START, (RENGA_END_OFF - RENGA_BLOCKS_START), 0);
		renga_work_commem_for_user(UNUSED_DUMP_PATH, RENGA_END_OFF, (0x1FE000 - RENGA_END_OFF), 0);
		printf(" framework dump: %s\n", RENGA_DUMP_PATH);
		printf(" sm blocks dump: %s\n", BLOCKS_DUMP_PATH);
		printf(" unused-mem dump: %s\n", UNUSED_DUMP_PATH);
	}
	
	renga_set_logging_for_user(0);

	return 0;
}

void wait_key_press()
{
	SceCtrlData pad;

	printf("Press START to continue.\n");

	while (1) {
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_START)
			break;
		if (pad.buttons & SCE_CTRL_SELECT) {
			do_dump = 1;
			break;
		}
		sceKernelDelayThread(200 * 1000);
	}
}
