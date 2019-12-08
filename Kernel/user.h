/*
	exec_code_for_user(path, csize)
	Reads [csize] from [path] to (0x1C000000 + NMFEND_OFF + 0x100) and jumps to it there
	ARG 1 (char *):
		- source payload file path, set to NULL if source is a buf
	ARG 2 (uint32_t):
		- payload size
	RET (int):
		- 0x23: file i/o error
		- renga_exec_code ret
*/
int renga_exec_code_for_user(const char *path, uint32_t csize) {
	char k_path[1 + (64 * 3)];
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	ksceKernelStrncpyUserToKernel(k_path, (uintptr_t)path, (64 * 3));
	LOG("Reading lv0 code from %s for user\n", k_path);
	SceIoStat stat;
	int stat_ret = ksceIoGetstat(k_path, &stat);
	if(stat_ret < 0)
		return 0x23;
	if (csize == 0)
		csize = stat.st_size;
	ret = renga_exec_code(NULL, k_path, csize, 0);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	work_commem_for_user(path, off, size, mode)
	[Copy/Read] to/from commem to/from [buf/file]
	ARG 1 (char *):
		- file path
	ARG 2 (uint32_t):
		- commem offset
	ARG 3 (uint32_t):
		- read/write size
	ARG 4 (int):
		- mode, if 1: file->commem, else commem->file
	RET (int):
		- NMPcopy/NMPfile_op ret
*/
int renga_work_commem_for_user(const char *path, uint32_t off, uint32_t csize, int mode) {
	char k_path[1 + (64 * 3)];
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("Working commem for user\n");
	ksceKernelStrncpyUserToKernel(k_path, (uintptr_t)path, (64 * 3));
	if (mode == 1) {
		SceIoStat stat;
		int stat_ret = ksceIoGetstat(k_path, &stat);
		if(stat_ret < 0)
			return 0x23;
		if (mode == 1)
			csize = stat.st_size;
	}
	ret = renga_work_commem(NULL, k_path, off, csize, mode);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	set_logging_for_user(mode)
	Sets logging mode for user
	ARG 1 (int):
		- new logging mode
	RET (int):
		- renga_set_logging ret
*/
int renga_set_logging_for_user(int mode) {
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("Setting log mode %d for user\n", mode);
	ret = renga_set_logging(mode);
	LOG("Set log mode %d for user\n", mode);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	force_reset_framework(restore_mirror)
	Resets the commem and sets up the lv0 framework for user
	ARG 1 (int):
		- if 1, executes reset entries
	RET (int):
		- renga_force_reset_framework ret
*/
int renga_force_reset_framework_for_user(int restore_mirror) {
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("WARNING: user requested lv0 framework reset!\n");
	ret = renga_force_reset_framework(restore_mirror);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	get_status_for_user(magic)
	Gets [magic] status for user
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- renga_get_status ret
*/
uint8_t renga_get_status_for_user(uint16_t magic) {
	uint32_t state;
	uint8_t ret = 0;
	ENTER_SYSCALL(state);
	LOG("User requested status for 0x%04X\n", magic);
	ret = renga_get_status(magic);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	get_opmode_for_user(magic)
	Gets [magic] status for user
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- renga_get_opmode ret
*/
uint8_t renga_get_opmode_for_user(uint16_t magic) {
	uint32_t state;
	uint8_t ret = 0;
	ENTER_SYSCALL(state);
	LOG("User requested opmode for 0x%04X\n", magic);
	ret = renga_get_opmode(magic);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	set_opmode_for_user(magic, opmode)
	Gets [magic] status for user
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	ARG 2 (uint8_t):
		- new entry mode
	RET (int):
		- renga_set_opmode ret
*/
int renga_set_opmode_for_user(uint16_t magic, uint8_t opmode) {
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("User sets opmode 0x%X for 0x%04X\n", opmode, magic);
	ret = renga_set_opmode(magic, opmode);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	force_commemblock_for_user(mode, clean)
	Reserves/Frees the commem
	ARG 1 (int):
		- if 1: reserve commem, else free commem
	ARG 2 (int):
		- memset? (0/1)
	RET (int):
		- renga_force_commemblock ret
*/
int renga_force_commemblock_for_user(int mode, int clean) {
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("WARNING: user requested commemblock %s [%d]!\n", mode == 1 ? "reserve" : "free", clean);
	ret = renga_force_commemblock(mode, clean);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	add_entry_for_user(path, type, magic)
	Adds a framework entry
	ARG 1 (char *):
		- entry file
	ARG 2 (uint8_t):
		- entry type 1/0x10 (code/SM)
	ARG 3 (uint16_t):
		- entry magic
	RET (int):
		- NMFadd_entry ret
*/
int renga_add_entry_for_user(char *path, uint8_t type, uint16_t magic) {
	char k_path[1 + (64 * 3)];
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	ksceKernelStrncpyUserToKernel(k_path, (uintptr_t)path, (64 * 3));
	LOG("Adding entry of type 0x%02X from %s for user\n", type, k_path);
	ret = renga_add_entry(NULL, k_path, 0, type, magic);
	EXIT_SYSCALL(state);
	return ret;
}

/*
	remove_entry_for_user(type, magic)
	Removes a framework entry
	ARG 1 (uint8_t):
		- entry type 1/0x10 (code/SM)
	ARG 2 (uint16_t):
		- entry magic
	RET (int):
		- NMFremove_entry ret
*/
int renga_remove_entry_for_user(uint8_t type, uint16_t magic) {
	uint32_t state;
	int ret = 0;
	ENTER_SYSCALL(state);
	LOG("Removing entry of type 0x%02X from 0x%02X for user\n", type, magic);
	ret = renga_remove_entry(type, magic);
	EXIT_SYSCALL(state);
	return ret;
}
