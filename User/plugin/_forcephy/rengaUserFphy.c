#include <psp2/kernel/modulemgr.h>
#include "../../../Include/renga_user-funcs.h"

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
  renga_xet_bank_for_user(2);
  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
  return SCE_KERNEL_STOP_SUCCESS;
}
