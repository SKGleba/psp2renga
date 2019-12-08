
/* 
	memcpy sample (include) for NMPRunner
	This code memcpies the 0x51A key from f00d keyring to 0x1C000000
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>
#include "../../Include/nmprunner.h"
#include "logging.h"

/*
	memcpy(*(u32_t *)0x1C010204, *(u32_t *)0x1C010200, *(u32_t *)0x1C010208);
*/
unsigned char mepcpy_payload[] = {
  0xf0, 0xcf, 0xd8, 0xff, 0x26, 0x4d, 0x22, 0x4e, 0x1a, 0x7b, 0x1e, 0x4b,
  0x00, 0x53, 0x16, 0x43, 0x16, 0xd3, 0x10, 0x80, 0x12, 0x43, 0x21, 0xc3,
  0x01, 0x1c, 0x34, 0xc3, 0x00, 0x02, 0x3e, 0x03, 0x0e, 0x43, 0x21, 0xc3,
  0x01, 0x1c, 0x34, 0xc3, 0x04, 0x02, 0x3e, 0x03, 0x0a, 0x43, 0x21, 0xc3,
  0x01, 0x1c, 0x34, 0xc3, 0x08, 0x02, 0x3e, 0x03, 0x06, 0x43, 0x13, 0x40,
  0x07, 0x43, 0x0f, 0x42, 0x0b, 0x41, 0x0f, 0x10, 0x16, 0x40, 0x00, 0x00,
  0x23, 0x4e, 0x27, 0x4d, 0x1f, 0x4b, 0x28, 0x4f, 0xbe, 0x10, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x1f, 0x00
};

static int hex_dump(unsigned char *addr, unsigned int size, char *name)
{
	LOG("hex_dump %s [%d]:\n ", name, size);
    unsigned int i;
    for (i = 0; i < (size >> 4); i++)
    {
        LOG("%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7], addr[8], addr[9], addr[10], addr[11], addr[12], addr[13], addr[14], addr[15]);
        addr += 0x10;
    }
	LOG("\n..ok!\n");
    return 0;
}

/*
	mepcpy(dst, src, sz)
	memcpy [sz] bytes from [src] to [dst]
	ARG 1 (uint32_t):
		- dst
	ARG 2 (uint32_t):
		- src
	ARG 3 (uint32_t)
		- sz
	RET (int):
		- 0x00: ok
		- 0x0X: exploit failed
		- 0x1X: commem already reserved
		- 0x2X: copy stage2 error
		- 0x3X: copy main error
		- 0x4X: payload execute error
		- 0x5X: commem already free
		- 0x6X: unsupported firmware for stage2
*/
static int mepcpy(uint32_t dst, uint32_t src, uint32_t sz) {
	int ret = 0;
	int sysroot = ksceKernelGetSysbase();
	uint32_t fw = *(uint32_t *)(*(int *)(sysroot + 0x6c) + 4);
	ret = NMPexploit_init(fw);
	if (ret != 0)
		return ret;
	ret = NMPconfigure_stage2(fw);
	if (ret != 0)
		return (0x60 + ret);
	ret = NMPreserve_commem(1);
	if (ret != 0)
		return (0x10 + ret);
	ret = NMPcopy(&NMPstage2_payload, 0x10000, sizeof(NMPstage2_payload), 0);
	if (ret != 0)
		return (0x20 + ret);
	ret = NMPcopy(&mepcpy_payload, 0x10100, sizeof(mepcpy_payload), 0);
	if (ret != 0)
		return (0x30 + ret);
	*(uint32_t *)(NMPcorridor + 0x10200) = src;
	*(uint32_t *)(NMPcorridor + 0x10204) = dst;
	*(uint32_t *)(NMPcorridor + 0x10208) = sz;
	ret = NMPfree_commem(0);
	if (ret != 0)
		return (0x50 + ret);
	ret = NMPf00d_jump((uint32_t)0x1C010000, fw);
	if (ret != 0)
		return (0x40 + ret);
	ksceSblSmCommStopSm(NMPctx, &NMPstop_res);
	return 0;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	LOG_START("memcpy_sample started!\n");
	int ret = 0;
	LOG("mepcpy-ing key 0x51A to 0x1C010000... ");
	ret = mepcpy(0x1C010000, (0xE0058000 + (0x51A * 0x20)), 0x20);
	LOG("0x%X\n", ret);
	NMPreserve_commem(0);
	hex_dump((NMPcorridor + 0x10000), 0x20, "key 0x51A");
	NMPfree_commem(1);
	LOG("memcpy_sample finished!\n");
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
