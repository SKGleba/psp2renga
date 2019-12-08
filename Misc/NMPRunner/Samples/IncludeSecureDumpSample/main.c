
/* 
	secure dump sample (include) for NMPRunner
	This code dumps f00d secure_kernel to "ux0:data/secure_dump_sample_sk.bin"
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>
#include "../../Include/nmprunner.h"
#include "logging.h"

// f00d keyring -> 0x1C000000
unsigned char cpy_sk[] = {
  0xa0, 0x6f, 0x16, 0x4d, 0x12, 0x4e, 0x1a, 0x7b, 0x0e, 0x4b, 0x00, 0x53,
  0x06, 0x43, 0x16, 0xd3, 0x10, 0x80, 0xfa, 0x03, 0xfe, 0x00, 0x01, 0xc3,
  0x00, 0x7e, 0x21, 0xc2, 0x80, 0x00, 0x21, 0xc1, 0x00, 0x1c, 0x0f, 0x10,
  0x06, 0x40, 0x00, 0x00, 0x13, 0x4e, 0x17, 0x4d, 0x0f, 0x4b, 0x60, 0x6f,
  0xbe, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x1f, 0x00
};

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	LOG_START("secure_dump_sample started!\n");
	int ret = 0;
	LOG("running payload... ");
	ret = NMPrun_default(&cpy_sk, sizeof(cpy_sk));
	LOG("0x%X\n", ret);
	LOG("getting results... ");
	NMPreserve_commem(0);
	ret = NMPfile_op("ux0:data/secure_dump_sample_sk.bin", 0, 0x7e00, 0);
	NMPfree_commem(1);
	LOG("0x%X\n", ret);
	LOG("dumped secure_kernel to ux0:data/secure_dump_sample_sk.bin\n");
	LOG("secure_dump_sample finished!\n");
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
