
/* 
	psp2renga by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <psp2kern/kernel/modulemgr.h>
#include <vitasdkkern.h>

#define LOG_LOC "ux0:data/0psp2renga.log"
#include "logging.h"

#include "../Include/nmfmanager.h"

unsigned char mepcpy_payload[] = {
  0xc0, 0x6f, 0x1a, 0x7b, 0x06, 0x4b, 0x1b, 0xc2, 0x09, 0x00, 0x1b, 0xc9,
  0x08, 0x00, 0x1b, 0xc3, 0x0a, 0x00, 0x46, 0x62, 0x90, 0x12, 0x1b, 0xc9,
  0x0b, 0x00, 0x86, 0x63, 0x30, 0x12, 0xc6, 0x69, 0x20, 0x19, 0x9e, 0xc0,
  0x0c, 0x00, 0x9e, 0x01, 0x9e, 0xc2, 0x04, 0x00, 0x9e, 0xc3, 0x08, 0x00,
  0x0f, 0xa0, 0x16, 0xd0, 0x10, 0x80, 0x0f, 0x10, 0x07, 0x4b, 0x40, 0x6f,
  0xbe, 0x10, 0x21, 0xc0, 0x05, 0xe0, 0x04, 0xc0, 0x24, 0x00, 0x0e, 0x09,
  0x95, 0xc9, 0x01, 0x00, 0x3b, 0xa9, 0x21, 0xc0, 0x05, 0xe0, 0x0a, 0x02,
  0x04, 0xc2, 0x04, 0x00, 0x2a, 0x01, 0x04, 0xc2, 0x08, 0x00, 0x2a, 0x03,
  0x04, 0xc3, 0x0c, 0x00, 0x01, 0xc2, 0x80, 0x20, 0x3a, 0x02, 0x04, 0xc3,
  0x1c, 0x00, 0x01, 0x52, 0x3a, 0x02, 0x04, 0xc3, 0x24, 0x00, 0x19, 0xe0,
  0x03, 0x00, 0x3e, 0x02, 0x25, 0xc2, 0x01, 0x00, 0x02, 0xa2, 0x3e, 0x00,
  0xb4, 0xbf, 0x21, 0xc9, 0x05, 0xe0, 0x94, 0xc9, 0x1c, 0x00, 0x00, 0x5a,
  0x9a, 0x0a, 0x19, 0xe0, 0x03, 0x00, 0x0e, 0x09, 0x95, 0xc9, 0x01, 0x00,
  0x02, 0xa9, 0xb0, 0xbf, 0xf0, 0xff, 0x1f, 0x00
};

static void *rcalls[16];
static int callcnt = 0;

/*
	exec_code(cbuf, floc, csize)
	Copies [csize]/Reads [csize] from [cbuf] to 0x1C000100 and jumps to it there
	ARG 1 (void *):
		- source payload buf, set to NULL if source is a file
	ARG 2 (char *):
		- source payload file path, set to NULL if source is a buf
	ARG 3 (uint32_t):
		- payload size
	RET (int):
		- NMFexec_code ret
*/
int renga_exec_code(void *cbuf, char *floc, uint32_t csize) {
	unsigned int ret = 0;
	LOG("Running the code... ");
	ret = NMFexec_code(cbuf, floc, csize);
	LOG("0x%X\n", ret);
	return ret;
}

/*
	remove_entry(type, magic)
	Removes a framework entry
	ARG 1 (uint8_t):
		- entry type 1/0x10 (code/SM)
	ARG 2 (uint16_t):
		- entry magic
	RET (int):
		- NMFremove_entry ret
*/
int renga_remove_entry(uint8_t type, uint16_t magic) {
	unsigned int ret = 0;
	LOG("Removing mem entry of type 0x%02X from 0x%04X... ", type, magic);
	ret = NMFremove_entry(type, magic);
	LOG("0x%X\n", ret);
	return ret;
}

/*
	add_entry(cbuf, file, csize, type, magic)
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
		- NMFadd_entry ret
*/
int renga_add_entry(void *cbuf, char *file, uint32_t csize, uint8_t type, uint16_t magic) {
	unsigned int ret = 0;
	if (cbuf != NULL) {
		LOG("Adding mem entry of type 0x%02X for 0x%04X... ", type, magic);
	} else
		LOG("Adding %s entry of type 0x%02X for 0x%04X... ", file, type, magic);
	ret = NMFadd_entry(cbuf, file, csize, type, magic);
	LOG("0x%X\n", ret);
	return ret;
}

/*
	set_opmode(magic, opmode)
	Sets operation mode for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	ARG 2 (uint8_t):
		- new entry mode
	RET (int):
		- NMFset_opmode ret
*/
int renga_set_opmode(uint16_t magic, uint8_t opmode) {
	unsigned int ret = 0;
	LOG("Setting opmode 0x%02X for 0x%04X... ", opmode, magic);
	ret = NMFset_opmode(magic, opmode);
	LOG("0x%X\n", ret);
	return ret;
}

/*
	get_opmode(magic)
	Gets operation mode for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- NMFget_opmode ret
*/
uint8_t renga_get_opmode(uint16_t magic) {
	uint8_t ret = 0;
	LOG("Getting opmode for 0x%04X... ", magic);
	ret = NMFget_opmode(magic);
	LOG("0x%02X\n", ret);
	return ret;
}

/*
	get_status(magic)
	Gets status for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- NMFget_status ret
*/
uint8_t renga_get_status(uint16_t magic) {
	uint8_t ret = 0;
	LOG("Getting status for 0x%04X... ", magic);
	ret = NMFget_status(magic);
	LOG("0x%02X\n", ret);
	return ret;
}

/*
	work_commem(buf, floc, off, size, mode)
	[Copy/Read] to/from commem to/from [buf/file]
	ARG 1 (void *):
		- buf, set to NULL if the source/target is a file
	ARG 2 (char *):
		- file path, set to NULL if the source/target is a buf
	ARG 3 (uint32_t):
		- commem offset
	ARG 4 (uint32_t):
		- copy/read size
	ARG 5 (int):
		- mode, if 1: source->commem, else commem->target
	RET (int):
		- NMPcopy/NMPfile_op ret
*/
int renga_work_commem(void *buf, char *floc, uint32_t off, uint32_t size, int mode) {
	unsigned int ret = 0;
	if (buf != NULL) {
		LOG("Copying 0x%lX %s buf %s commem+0x%lX... ", size, (mode == 1) ? "FROM" : "TO", (mode == 1) ? "TO" : "FROM", off);
		if (mode == 1) {
			ret = NMPcopy(buf, off, size, 0);
		} else
			ret = NMPcopy(buf, off, size, 1);
		LOG("0x%X\n", ret);
	} else {
		LOG("%s 0x%lX %s %s %s commem+0x%lX... ", (mode == 1) ? "READING" : "WRITING", size, (mode == 1) ? "FROM" : "TO", floc, (mode == 1) ? "TO" : "FROM", off);
		ret = NMPfile_op(floc, off, size, mode);
		LOG("0x%X\n", ret);
	}
	return ret;
}

/*
	force_commemblock(mode, clean)
	Reserves/Frees the commem
	ARG 1 (int):
		- if 1: reserve commem, else free commem
	ARG 2 (int):
		- memset? (0/1)
	RET (int):
		- NMPreserve_commem / NMPfree_commem ret
*/
int renga_force_commemblock(int mode, int clean) {
	unsigned int ret = 0;
	LOG("Forcing commem %s and %s0-ing it... ", (mode == 1) ? "RESERVE" : "FREE", (clean == 0) ? "NOT " : "");
	if (mode == 1) {
		ret = NMPreserve_commem(clean);
	} else {
		ret = NMPfree_commem(clean);
	}
	LOG("0x%X\n", ret);
	return ret;
}

/*
	set_logging(mode)
	Sets logging mode
	ARG 1 (int):
		- new logging mode
	RET (int):
		- logging mode
*/
int renga_set_logging(int mode) {
	if (mode == 0)
		LOG("logging disabled!\n");
	enable_logging = mode;
	if (mode == 1)
		LOG_START("logging enabled!\n\n ----Welcome to PSP2RENGA----\n\n");
	return enable_logging;
}


/*
	add_reset_entry(func, cln_slot)
	Adds a framework-reset entry (or cleans if func=NULL)
	ARG 1 (void *):
		- pointer to the function that will be executed at framework reset,
		set to NULL if in clean mode
	ARG 2 (int):
		- slot to clean
	RET (int):
		- 0x69: No free slot
		- if func == NULL: current slot count
		- if func != NULL: func's slot
*/
int renga_add_reset_entry(void *vaddr, int cln_slot) {
	int cur_slot = 0;
	if (vaddr != NULL) {
		LOG("Adding reset func to 0x%X... ", callcnt);
		if (callcnt < 16) {
			rcalls[callcnt] = vaddr;
			cur_slot = callcnt;
			callcnt = callcnt + 1;
		} else
			cur_slot = 0x69;
	} else {
		LOG("Wiping reset func [0x%X]... ", cln_slot);
		rcalls[cln_slot] = NULL;
		if ((callcnt - 1) > cln_slot) {
			callcnt = callcnt - 1;
			rcalls[cln_slot] = rcalls[callcnt];
			rcalls[callcnt] = NULL;
		}
		cur_slot = callcnt - 1;
	}
	LOG("0x%X\n", cur_slot);
	return cur_slot;
}

/*
	force_reset_framework(rexploit)
	Resets the commem and sets up the framework
	ARG 1 (int):
		- if 1, re-run the exploit
	RET (int):
		- NMFsetup_framework ret
*/
int renga_force_reset_framework(int rexploit) {
	unsigned int ret = 0, tmpctr = 0;
	renga_force_commemblock(0, 0);
	renga_force_commemblock(1, 1);
	LOG("WARNING: lv0 framework reset requested... ");
	(rexploit == 1) ? NMPctx = -1 : renga_set_opmode(RENGA_MAGIC_MASTER, 1);
	ret = NMFsetup_framework(rexploit);
	LOG("0x%X\n", ret);
	while (tmpctr < callcnt) {
		if (rcalls[tmpctr] != NULL) {
			LOG("Running 0x%X... ", tmpctr);
			void (*curtmp)() = (void*)(rcalls[tmpctr]);
			curtmp();
			LOG("OK\n");
		} else
			break;
		tmpctr = tmpctr + 1;
	}
	return ret;
}

/*
	xet_bank(entry)
	Sets/Gets current framework bank
	ARG 1 (int):
		- new bank (0 to GET current bank)
	RET (int):
		- 0x00: ok
		- 0x35: commem not reserved
		- 0x69: invalid target bank
		- else: (if entry==0 : current bank)
*/
int renga_xet_bank(int entry) {
	unsigned int ret = 0;
	LOG("Bank %s requested... ", (entry == 0) ? "info" : "switch");
	ret = NMFxet_bank(entry);
	LOG("0x%X\n", ret);
	return ret;
}

/*
	mepcpy(dst, src, sz, device)
	Adds a framework-reset entry (or cleans if func=NULL)
	ARG 1 (uint32_t):
		- dst paddr
	ARG 2 (uint32_t):
		- src paddr
	ARG 3 (uint32_t):
		- size to copy
	ARG 4 (uint32_t):
		- memcpy func (0 for secure_kernel's; 1 for the second cry's (bigmac))
	RET (int):
		- if device == 0: dst or error
		- if device == 1: 0 or error
*/
int renga_mepcpy(uint32_t dst, uint32_t src, uint32_t sz, uint32_t device) {
	volatile NMFfm_nfo *fmnfo = (void *)NMPcorridor;
	fmnfo->cucnfo.unused = (NMFcurrent_paddr + NMFEND_OFF + 0x100 + sizeof(mepcpy_payload) + 0x10);
	*(uint32_t *)(NMPcorridor + NMFEND_OFF + 0x100 + sizeof(mepcpy_payload) + 0x10) = dst;
	*(uint32_t *)(NMPcorridor + NMFEND_OFF + 0x100 + sizeof(mepcpy_payload) + 0x14) = src;
	*(uint32_t *)(NMPcorridor + NMFEND_OFF + 0x100 + sizeof(mepcpy_payload) + 0x18) = sz;
	*(uint32_t *)(NMPcorridor + NMFEND_OFF + 0x100 + sizeof(mepcpy_payload) + 0x1C) = device;
	return renga_exec_code(&mepcpy_payload, NULL, sizeof(mepcpy_payload));
}

#include "user.h"

// Thanks CelesteBlue
SceUID sub_81000000_patched_hook = -1;
static tai_hook_ref_t sub_81000000_patched_ref;
SceUID sub_81000000_patched(int resume, int eventid, void *args, void *opt) {
	int ret = TAI_CONTINUE(SceUID, sub_81000000_patched_ref, resume, eventid, args, opt);
	if (eventid == 0x100000) {
		if (((renga_xet_bank(0) == 1) && renga_get_status(0) == 0x22) || renga_xet_bank(0) == 2)
			renga_force_reset_framework(1);
	}
	return ret;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	LOG_START("psp2renga started!\n");
	int ret = 0;
	ret = renga_force_commemblock(1, 1);
	if (ret != 0)
		return SCE_KERNEL_START_FAILED;
	NMFramework_uid = 0;
	NMForig_corridor = NMPcorridor;
	
	// Comment this to start off camera SRAM
	//--
	alloc_phycont(NMPcorridor_size, 4096, &NMFramework_uid, &NMFramework);
	ksceKernelGetPaddr(NMFramework, &NMFramework_paddr);
	NMPcorridor = NMFramework;
	NMFcurrent_paddr = NMFramework_paddr;
	//--
	
	ret = NMFsetup_framework(1);
	LOG("setup_lv0_framework: 0x%X\n", ret);
	if (ret != 0)
		return SCE_KERNEL_START_FAILED;
	tai_module_info_t scesblssmgr_modinfo;
	taiGetModuleInfoForKernel(KERNEL_PID, "SceSblSsMgr", &scesblssmgr_modinfo);
	sub_81000000_patched_hook = taiHookFunctionOffsetForKernel(KERNEL_PID, &sub_81000000_patched_ref, scesblssmgr_modinfo.modid, 0, 0, 1, sub_81000000_patched);
	LOG("Added resume patch\n");
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	renga_force_commemblock(1, 0);
	renga_force_commemblock(0, 1);
	LOG("psp2renga finished:?!?\n");
	return SCE_KERNEL_STOP_SUCCESS;
}

