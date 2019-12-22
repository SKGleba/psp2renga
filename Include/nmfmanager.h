/* 
	NMFManager by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/

#include "nmprunner.h"
#include "renga-defs.h"

#define NMFMGR_OFF (0x200)
#define NMFBLOCK_START RENGA_BLOCKS_START
#define NMFBLOCK_SZ RENGA_BLOCK_SZ
#define NMFBCODE_OFF RENGA_MAX_SM_SZ
#define NMFEND_OFF (0xD0000)

static void *NMFramework;
static void *NMForig_corridor;
static uintptr_t NMFramework_paddr;
static SceUID NMFramework_uid = 0;
static uintptr_t NMFcurrent_paddr;

unsigned char NMFloadsm_hook[] = {
  0xc0, 0x6f, 0x80, 0xd3, 0x9e, 0x80, 0x1a, 0x7b, 0x06, 0x45, 0xfa, 0x0b,
  0x3e, 0x05, 0x5b, 0xc3, 0x02, 0x00, 0x14, 0xe3, 0x13, 0x00, 0x5b, 0xc2,
  0x05, 0x00, 0x5b, 0xc0, 0x04, 0x00, 0x5b, 0xc3, 0x06, 0x00, 0x46, 0x62,
  0x00, 0x12, 0x5b, 0xc0, 0x07, 0x00, 0x86, 0x63, 0x30, 0x12, 0xc6, 0x60,
  0x20, 0x10, 0x0f, 0x10, 0x58, 0xc0, 0x03, 0x00, 0x05, 0x51, 0x10, 0xd3,
  0x20, 0x80, 0x3f, 0x10, 0x07, 0x45, 0xfe, 0x0b, 0x40, 0x6f, 0xbe, 0x10,
  0xf0, 0xff, 0x1f, 0x00
};

unsigned char NMFinject_payload[] = {
  0x80, 0x6f, 0x16, 0x45, 0x12, 0x46, 0x1e, 0x4d, 0x1a, 0x4e, 0x1a, 0x7b,
  0x0e, 0x4b, 0xd0, 0x05, 0xe0, 0x06, 0x00, 0x53, 0x06, 0x43, 0x16, 0xd3,
  0x10, 0x80, 0xfa, 0x03, 0xfe, 0x00, 0x50, 0x53, 0x21, 0xc2, 0x85, 0x1f,
  0x24, 0xc2, 0x00, 0x04, 0x50, 0xd1, 0x7c, 0x80, 0x50, 0x0d, 0x60, 0x0e,
  0x0f, 0x10, 0x06, 0x40, 0xfe, 0x00, 0x01, 0xc3, 0x80, 0x00, 0x21, 0xc2,
  0x85, 0x1f, 0x24, 0xc2, 0x00, 0x08, 0x00, 0xd1, 0x9e, 0x80, 0x50, 0x0d,
  0x60, 0x0e, 0x0f, 0x10, 0x06, 0x40, 0x80, 0xd2, 0x9e, 0x80, 0x21, 0xc3,
  0x85, 0x1f, 0x34, 0xc3, 0x00, 0x0b, 0x3e, 0x03, 0x2a, 0x03, 0x80, 0xd3,
  0x9e, 0x80, 0x3e, 0x03, 0x3e, 0xa3, 0x06, 0xd3, 0x0a, 0x80, 0x01, 0xc2,
  0x50, 0xd0, 0x39, 0x02, 0x08, 0xd3, 0x0a, 0x80, 0x01, 0xc2, 0x7c, 0x80,
  0x39, 0x02, 0x0a, 0xd3, 0x0a, 0x80, 0x01, 0xc2, 0x0f, 0x10, 0x39, 0x02,
  0x72, 0xd3, 0x03, 0x80, 0x01, 0xc2, 0x79, 0xdc, 0x39, 0x02, 0x74, 0xd3,
  0x03, 0x80, 0x01, 0xc2, 0x9a, 0x00, 0x39, 0x02, 0xfe, 0xd3, 0x0a, 0x80,
  0x01, 0xc2, 0x10, 0x06, 0x39, 0x02, 0x80, 0xd3, 0x9e, 0x80, 0x3e, 0x03,
  0x06, 0x43, 0x07, 0x43, 0x30, 0x00, 0x1b, 0x4e, 0x1f, 0x4d, 0x13, 0x46,
  0x17, 0x45, 0x0f, 0x4b, 0x20, 0x4f, 0xbe, 0x10, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0xff, 0x1f, 0x00
};

unsigned char NMFmanager[] = {
  0x80, 0x6f, 0x80, 0xd3, 0x9e, 0x80, 0x1a, 0x7b, 0x16, 0x45, 0x12, 0x46,
  0x0e, 0x47, 0x0a, 0x48, 0x06, 0x4b, 0x3e, 0x06, 0x34, 0x53, 0x6b, 0xc8,
  0x03, 0x00, 0x35, 0xe8, 0x72, 0x00, 0x21, 0xca, 0x80, 0x00, 0x12, 0x53,
  0x60, 0xc9, 0x16, 0x00, 0x00, 0x50, 0xa4, 0xca, 0x0e, 0xb0, 0x0c, 0x51,
  0x68, 0xc3, 0x03, 0x00, 0x09, 0xe1, 0x15, 0x00, 0x00, 0xc2, 0x02, 0x00,
  0x1e, 0x62, 0x22, 0x96, 0x05, 0xc7, 0xff, 0x00, 0x2b, 0xc3, 0x05, 0x00,
  0x2b, 0xcb, 0x04, 0x00, 0x46, 0x63, 0xb0, 0x13, 0x14, 0xa3, 0xaf, 0x02,
  0x35, 0xe2, 0x05, 0x00, 0x9b, 0x03, 0x04, 0xe3, 0x50, 0x00, 0x04, 0x60,
  0x20, 0x69, 0x0d, 0x57, 0x00, 0x53, 0x13, 0x52, 0x68, 0xc2, 0x03, 0x00,
  0x14, 0xe3, 0x3c, 0x00, 0x1f, 0x67, 0x05, 0x96, 0x34, 0x53, 0x58, 0xc3,
  0x17, 0x00, 0x5c, 0xc3, 0x16, 0x00, 0x10, 0x52, 0x35, 0xc3, 0xf0, 0x00,
  0x25, 0xe3, 0x1f, 0x00, 0x5b, 0xc3, 0x19, 0x00, 0x5b, 0xc1, 0x18, 0x00,
  0x80, 0xd2, 0x9e, 0x80, 0x87, 0x67, 0x46, 0x63, 0x10, 0x13, 0x5b, 0xc1,
  0x1a, 0x00, 0x2e, 0x02, 0x86, 0x61, 0x30, 0x11, 0x5b, 0xc3, 0x1b, 0x00,
  0x20, 0xc2, 0x00, 0x05, 0x02, 0x92, 0x16, 0xd0, 0x10, 0x80, 0xc6, 0x63,
  0x10, 0x13, 0x00, 0xd1, 0xb0, 0x80, 0x0f, 0x10, 0x69, 0x53, 0x58, 0xc3,
  0x17, 0x00, 0x5c, 0xc3, 0x16, 0x00, 0x35, 0xc3, 0x0f, 0x00, 0x14, 0xe3,
  0x0d, 0x00, 0x80, 0xd3, 0x9e, 0x80, 0x86, 0x67, 0x3e, 0x00, 0x00, 0xd3,
  0xe5, 0x00, 0x30, 0x90, 0x70, 0x90, 0x0f, 0x10, 0x58, 0xc0, 0x17, 0x00,
  0x14, 0x53, 0x68, 0xc3, 0x03, 0x00, 0x80, 0x00, 0x0f, 0x47, 0x0b, 0x48,
  0x13, 0x46, 0x17, 0x45, 0x07, 0x4b, 0x20, 0x4f, 0xbe, 0x10, 0x01, 0x53,
  0x6a, 0xbf, 0x11, 0x58, 0xea, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0xff, 0x1f, 0x00
};

unsigned char NMFfcmdh_hook[] = {
  0x21, 0xc3, 0x00, 0xe0, 0xc0, 0x6f, 0x34, 0xc3, 0x10, 0x00, 0x1a, 0x7b,
  0x06, 0x45, 0xfa, 0x0b, 0x3e, 0x02, 0x80, 0xd3, 0x9e, 0x80, 0x3e, 0x05,
  0x5b, 0xc0, 0x0d, 0x00, 0x5b, 0xc9, 0x0e, 0x00, 0x5b, 0xc3, 0x0c, 0x00,
  0x46, 0x60, 0x86, 0x69, 0x30, 0x10, 0x90, 0x10, 0x5b, 0xc9, 0x0f, 0x00,
  0xc6, 0x69, 0x90, 0x10, 0x3a, 0xa0, 0x50, 0x01, 0x0f, 0x10, 0x00, 0x02,
  0x00, 0x53, 0x42, 0x62, 0x58, 0xc2, 0x11, 0x00, 0x00, 0x02, 0x58, 0xc0,
  0x10, 0x00, 0x82, 0x62, 0xc2, 0x60, 0x58, 0xc2, 0x12, 0x00, 0x58, 0xc0,
  0x13, 0x00, 0x58, 0xc3, 0x0c, 0x00, 0x58, 0xc3, 0x0d, 0x00, 0x58, 0xc3,
  0x0e, 0x00, 0x58, 0xc3, 0x0f, 0x00, 0x07, 0x45, 0xfe, 0x0b, 0x40, 0x6f,
  0xbe, 0x10, 0x20, 0x01, 0xdc, 0xd3, 0x0a, 0x80, 0x3f, 0x10, 0xf0, 0xbf,
  0xf0, 0xff, 0x1f, 0x00
};

unsigned char NMFswap_bank[] = {
  0xc0, 0x6f, 0x06, 0x41, 0x80, 0xd3, 0x9e, 0x80, 0x3e, 0x03, 0x0e, 0x43,
  0x80, 0xd2, 0x9e, 0x80, 0x0f, 0x43, 0x3b, 0xc1, 0x08, 0x00, 0x3b, 0xc0,
  0x09, 0x00, 0x46, 0x60, 0x00, 0x11, 0x3b, 0xc0, 0x0a, 0x00, 0x86, 0x60,
  0x00, 0x11, 0x3b, 0xc3, 0x0b, 0x00, 0xc6, 0x63, 0x10, 0x13, 0x2a, 0x03,
  0x01, 0xc3, 0x34, 0x69, 0x30, 0x00, 0x40, 0x6f, 0x02, 0x70, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x1f, 0x00
};

#define ALIGN(x, a)	(((x) + ((a) - 1)) & ~((a) - 1))

int alloc_phycont(unsigned int size, unsigned int alignment, SceUID *uid, void **addr)
{
	int ret;
	SceUID mem_uid;
	void *mem_addr;

	SceKernelAllocMemBlockKernelOpt opt;
	memset(&opt, 0, sizeof(opt));
	opt.size = sizeof(opt);
	opt.attr = SCE_KERNEL_ALLOC_MEMBLOCK_ATTR_PHYCONT | SCE_KERNEL_ALLOC_MEMBLOCK_ATTR_HAS_ALIGNMENT;
	opt.alignment = ALIGN(alignment, 0x1000);
	mem_uid = ksceKernelAllocMemBlock("phycont", 0x30808006, ALIGN(size, 0x1000), &opt);
	if (mem_uid < 0)
		return mem_uid;

	ret = ksceKernelGetMemBlockBase(mem_uid, &mem_addr);
	if (ret < 0) {
		ksceKernelFreeMemBlock(mem_uid);
		return ret;
	}

	if (uid)
		*uid = mem_uid;
	if (addr)
		*addr = mem_addr;

	return 0;
}

/*
	setup_framework(rexploit)
	Setup the NMFramework
	ARG 1 (int):
		- if 1, re-run the exploit
	RET (int):
		- 0x00: ok
		- 0x0X: exploit failed
		- 0x1X: enable framework error
		- 0x2X: copy stage2 error
		- 0x3X: copy/set main error
		- 0x4X: payload execute error
		- 0x6X: unsupported firmware for stage2
		- 0x7X: sm_load framework copy error
		- 0x8X: manager copy error
		- 0x9X: irq_user framework copy error
*/
static int NMFsetup_framework(int rexploit) {
	int ret = 0;
	if (rexploit == 1) {
		int sysroot = ksceKernelGetSysbase();
		uint32_t fw = *(uint32_t *)(*(int *)(sysroot + 0x6c) + 4);
		NMPcache_ussm("os0:sm/update_service_sm.self", 1);
		ret = NMPexploit_init(fw);
		if (ret != 0)
			return ret;
		NMPfree_commem(0);
		NMPcorridor_paddr = 0x1f850000;
		NMPcorridor_size = 0x10000;
		NMPreserve_commem(1);
		ret = NMPconfigure_stage2(fw);
		if (ret != 0)
			return (0x60 + ret);
		ret = NMPcopy(&NMPstage2_payload, 0, sizeof(NMPstage2_payload), 0);
		if (ret != 0)
			return (0x20 + ret);
		ret = NMPcopy(&NMFinject_payload, 0x100, sizeof(NMFinject_payload), 0);
		if (ret != 0)
			return (0x30 + ret);
		ret = NMPcopy(&NMFloadsm_hook, 0x400, sizeof(NMFloadsm_hook), 0);
		if (ret != 0)
			return (0x70 + ret);
		ret = NMPcopy(&NMFfcmdh_hook, 0x800, sizeof(NMFfcmdh_hook), 0);
		if (ret != 0)
			return (0x90 + ret);
		*(uint32_t *)(NMPcorridor + 0xB00) = NMFcurrent_paddr;
		ret = NMPf00d_jump((uint32_t)0x1f850000, fw);
		if (ret != 0)
			return (0x40 + ret);
		ksceSblSmCommStopSm(NMPctx, &NMPstop_res);
		NMPfree_commem(1);
		NMPcorridor_paddr = NMFcurrent_paddr;
		NMPcorridor_size = 0x1FE000;
		NMPreserve_commem(0);
	}
	memset(NMPcorridor, 0, NMPcorridor_size);
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	fmnfo->magic = 0x6934;
	fmnfo->mgrpaddr = (NMFcurrent_paddr + NMFMGR_OFF);
	fmnfo->status = 0x34;
	memcpy((void *)(NMPcorridor + NMFMGR_OFF), &NMFmanager, sizeof(NMFmanager));
	fmnfo->do_use = 1;
	*(uint16_t *)(NMPcorridor + 0x1FD0FE) = 0x6934;
	return 0;
}

/*
	add_entry(cbuf, file, rsz, type, magic)
	Adds a framework entry
	ARG 1 (void *):
		- entry buf, set to NULL if the source is a file
	ARG 2 (char *):
		- entry file, set to NULL if the source is a buf
	ARG 3 (uint32_t):
		- entry sz
	ARG 4 (uint8_t):
		- entry type 1/0x10 (code/SM)
	ARG 5 (uint16_t):
		- entry magic
	RET (int):
		- 0x00: ok
		- 0x21: entry error (reserved/no free slot)
		- 0x22: unsupported type
		- 0x23: file error
		- 0x35: commem not reserved
*/
static int NMFadd_entry(void *cbuf, char *file, uint32_t rsz, uint8_t type, uint16_t magic) {
	if (NMPcorridor == NULL)
		return 0x35;
	int curetr = 0, maxetr = RENGA_ENTRIES_MAX, found = 0;
	uint32_t off = NMFBLOCK_START;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	while (curetr < maxetr) {
		if (fmnfo->entries[curetr].magic == 0) {
			found = 1;
			maxetr = 0;
			break;
		} else if (fmnfo->entries[curetr].magic == magic && (fmnfo->entries[curetr].do_use & type) == 0) {
			found = 1;
			maxetr = 0;
			break;
		} else {
			if (fmnfo->entries[curetr].magic == magic && (fmnfo->entries[curetr].do_use & type) != 0)
				break;
			curetr = curetr + 1;
		}
	}
	if (found == 0)
		return 0x21;
	if (type != RENGA_TYPE_CODE && type != RENGA_TYPE_SM)
		return 0x22;
	off = off + (curetr * NMFBLOCK_SZ);
	if (type == RENGA_TYPE_CODE)
		off = off + NMFBCODE_OFF;
	if (cbuf != NULL) {
		memcpy((NMPcorridor + off), cbuf, rsz);
	} else {
		SceIoStat stat;
		int stat_ret = ksceIoGetstat(file, &stat), fd = 0;
		if(stat_ret < 0)
			return 0x23;
		if (type == RENGA_TYPE_CODE) {
			rsz = stat.st_size;
		} else if (type == RENGA_TYPE_SM) {
			rsz = (stat.st_size - 0x1000);
		}
		fd = ksceIoOpen(file, SCE_O_RDONLY, 0);
		if (type == RENGA_TYPE_SM)
			ksceIoLseek(fd, 0x1000, 0);
		ksceIoRead(fd, (NMPcorridor + off), rsz);
		ksceIoClose(fd);
	}
	if (type == RENGA_TYPE_SM) {
		fmnfo->entries[curetr].smsz = rsz;
		fmnfo->entries[curetr].do_use = fmnfo->entries[curetr].do_use + RENGA_TYPE_SM;
	} else if (type == RENGA_TYPE_CODE) {
		fmnfo->entries[curetr].do_use = fmnfo->entries[curetr].do_use + RENGA_TYPE_CODE;
	}
	fmnfo->entries[curetr].magic = magic;
	return 0;
}

/*
	remove_entry(type, magic)
	Removes a framework entry
	ARG 1 (uint8_t):
		- entry type 1/0x10 (code/SM)
	ARG 2 (uint16_t):
		- entry magic
	RET (int):
		- 0x00: ok
		- 0x21: entry error (not found)
		- 0x22: unsupported type
		- 0x35: commem not reserved
*/
static int NMFremove_entry(uint8_t type, uint16_t magic) {
	if (NMPcorridor == NULL)
		return 0x35;
	int curetr = 0, maxetr = RENGA_ENTRIES_MAX, found = 0;
	uint32_t off = NMFBLOCK_START, rsz = 0;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	while (curetr < maxetr) {
		if (fmnfo->entries[curetr].magic == magic && (fmnfo->entries[curetr].do_use & type) != 0) {
			found = curetr + 1;
		} else if (fmnfo->entries[curetr].magic == 0) {
			maxetr = curetr - 1;
			break;
		}
		curetr = curetr + 1;
	}
	if (found == 0)
		return 0x21;
	curetr = found - 1;
	if (type != RENGA_TYPE_CODE && type != RENGA_TYPE_SM)
		return 0x22;
	off = off + (curetr * NMFBLOCK_SZ);
	if (type == RENGA_TYPE_SM) {
		rsz = RENGA_MAX_SM_SZ;
		fmnfo->entries[curetr].smsz = 0;
		fmnfo->entries[curetr].do_use = type & 0x0f;
		memset((void *)(NMPcorridor + off), 0, rsz);
	} else if (type == RENGA_TYPE_CODE) {
		off = off + RENGA_MAX_SM_SZ;
		rsz = RENGA_BLOCK_SZ - RENGA_MAX_SM_SZ;
		fmnfo->entries[curetr].do_use = type & 0xf0;
		memset((void *)(NMPcorridor + off), 0, rsz);
	}
	if (fmnfo->entries[curetr].do_use == 0 && fmnfo->entries[maxetr].do_use > 0) {
		off = NMFBLOCK_START + (curetr * NMFBLOCK_SZ);
		rsz = NMFBLOCK_START + (maxetr * NMFBLOCK_SZ);
		memcpy((void *)(NMPcorridor + off), (void *)(NMPcorridor + rsz), NMFBLOCK_SZ);
		fmnfo->entries[curetr].smsz = fmnfo->entries[maxetr].smsz;
		fmnfo->entries[curetr].do_use = fmnfo->entries[maxetr].do_use;
		fmnfo->entries[curetr].magic = fmnfo->entries[maxetr].magic;
		fmnfo->entries[maxetr].smsz = 0;
		fmnfo->entries[maxetr].do_use = 0;
		fmnfo->entries[maxetr].magic = 0;
		memset((void *)(NMPcorridor + rsz), 0, NMFBLOCK_SZ);
	}
	return 0;
}

/*
	set_opmode(magic, opmode)
	Sets operation mode for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	ARG 2 (uint8_t):
		- new entry mode
	RET (int):
		- 0x00: ok
		- 0x21: entry error (not exists)
		- 0x35: commem not reserved
*/
static int NMFset_opmode(uint16_t magic, uint8_t opmode) {
	if (NMPcorridor == NULL)
		return 0x35;
	int curetr = 0, maxetr = RENGA_ENTRIES_MAX, found = 0;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	if (magic == 0) {
		fmnfo->do_use = opmode;
		maxetr = 0;
	} else {
		while (curetr < maxetr) {
			if (fmnfo->entries[curetr].magic == magic) {
				found = 1;
				maxetr = 0;
				break;
			} else 
				curetr = curetr + 1;
		}
		if (found == 1) {
			fmnfo->entries[curetr].do_use = opmode;
		} else 
			maxetr = 0x21;
	}
	return maxetr;
}

/*
	get_opmode(magic)
	Gets operation mode for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- 0x00: ok
		- 0x21: entry error (not exists)
		- 0x22: framework is NOT present (for magic 0x0)
		- 0x35: commem not reserved
*/
static uint8_t NMFget_opmode(uint16_t magic) {
	if (NMPcorridor == NULL)
		return 0x35;
	int curetr = 0, maxetr = RENGA_ENTRIES_MAX, found = 0;
	uint8_t opmode = 0;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	if (magic == 0) {
		if (fmnfo->magic == 0x6934 && *(uint32_t *)(NMPcorridor + 0x1FD0FE) == 0x6934) {
			opmode = fmnfo->do_use;
		} else if (fmnfo->magic == 0x6934 && *(uint32_t *)(NMPcorridor + 0x1FD0FE) != 0x6934) {
			opmode = 0x28;
		} else
			opmode = 0x22;
	} else {
		while (curetr < maxetr) {
			if (fmnfo->entries[curetr].magic == magic) {
				found = 1;
				maxetr = 0;
				break;
			} else 
				curetr = curetr + 1;
		}
		if (found == 1) {
			opmode = fmnfo->entries[curetr].do_use;
		} else 
			opmode = 0x21;
	}
	return opmode;
}

/*
	get_status(magic)
	Gets status for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- 0x00: ok
		- 0x21: entry error (not exists)
		- 0x22: framework is NOT present (for magic 0x0)
		- 0x35: commem not reserved
*/
static uint8_t NMFget_status(uint16_t magic) {
	if (NMPcorridor == NULL)
		return 0x35;
	int curetr = 0, maxetr = RENGA_ENTRIES_MAX, found = 0;
	uint8_t opmode = 0;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	if (magic == 0) {
		if (fmnfo->magic == 0x6934 && fmnfo->do_use == 1) {
			opmode = fmnfo->status;
		} else if (fmnfo->magic == 0x6934 && fmnfo->do_use == 0) {
			opmode = 0x27;
		} else
			opmode = 0x22;
	} else {
		while (curetr < maxetr) {
			if (fmnfo->entries[curetr].magic == magic) {
				found = 1;
				maxetr = 0;
				break;
			} else 
				curetr = curetr + 1;
		}
		if (found == 1) {
			opmode = fmnfo->entries[curetr].status;
		} else
			opmode = 0x21;
	}
	return opmode;
}

/*
	exec_code(cbuf, floc, csize)
	Copies [csize]/Reads [csize] from [cbuf] to 0x1C000100 and lv0-jumps to it there
	ARG 1 (void *):
		- source payload buf, set to NULL if source is a file
	ARG 2 (char *):
		- source payload file path, set to NULL if source is a buf
	ARG 3 (uint32_t):
		- payload size
	RET (int):
		- 0x2X: copy/read error
		- 0x35: commem not reserved
		- 0xDEADBEEF: payload execute error/no ret
		- else: payload ret
*/
static int NMFexec_code(void *cbuf, char *floc, uint32_t csize)
{
	if (NMPcorridor == NULL)
		return 0x35;
    int ret;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	if (cbuf != NULL) {
		ret = NMPcopy(cbuf, (NMFEND_OFF + 0x100), csize, 0);
	} else
		ret = NMPfile_op(floc, (NMFEND_OFF + 0x100), csize, 1);
	if (ret != 0)
		return (0x20 + ret);
	fmnfo->cucnfo.resp = 0xDEADBEEF;
	fmnfo->cucnfo.paddr = (NMFcurrent_paddr + NMFEND_OFF + 0x100);
	ksceSblSmSchedProxyExecuteF00DCommand(0, 0, 0, 0);
	fmnfo->cucnfo.paddr = 0;
    return fmnfo->cucnfo.resp;
}
		
/*
	xet_bank(entry)
	Sets current framework bank
	ARG 1 (int):
		- new bank (0 to GET current bank)
	RET (int):
		- 0x00: ok
		- 0x35: commem not reserved
		- 0x69: invalid target bank
		- else: (if entry==0 : current bank)
*/
static int NMFxet_bank(int bank) {
	if (NMPcorridor == NULL)
		return 0x35;
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	if (bank == 0) {
		return (NMFcurrent_paddr == 0x1C000000) ? 1 : 2;
	} else if (bank == 1 && NMFcurrent_paddr != 0x1C000000) {
		fmnfo->cucnfo.unused = 0x1C000000;
		fmnfo->mgrpaddr = (0x1C000000 + NMFMGR_OFF);
		memcpy((void *)NMForig_corridor, (void *)NMPcorridor, NMFEND_OFF);
		NMFexec_code(&NMFswap_bank, NULL, sizeof(NMFswap_bank));
		NMPcorridor = NMForig_corridor;
		NMFcurrent_paddr = 0x1C000000;
		ksceKernelFreeMemBlock(NMFramework_uid);
		NMFramework_uid = 0;
	} else if (bank == 2 && NMFcurrent_paddr == 0x1C000000 && NMFramework_uid == 0) {
		alloc_phycont(NMPcorridor_size, 4096, &NMFramework_uid, &NMFramework);
		ksceKernelGetPaddr(NMFramework, &NMFramework_paddr);
		fmnfo->cucnfo.unused = NMFramework_paddr;
		fmnfo->mgrpaddr = (NMFramework_paddr + NMFMGR_OFF);
		memcpy((void *)NMFramework, (void *)NMPcorridor, NMFEND_OFF);
		NMFexec_code(&NMFswap_bank, NULL, sizeof(NMFswap_bank));
		NMPcorridor = NMFramework;
		NMFcurrent_paddr = NMFramework_paddr;
	} else
		return 0x69;
	return 0;
}