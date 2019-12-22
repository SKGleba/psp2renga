/*
	exec_code_for_user(path, csize)
	Reads [csize] from [path] to (commem + NMFEND_OFF + 0x100) and jumps to it there.
	The code is executed with two args:
		- ARG 1 (uint32_t): commem paddr
		- ARG 2 (uint): requested fcmd, should be 0
	ARG 1 (char *):
		- source payload file path, set to NULL if source is a buf
	ARG 2 (uint32_t):
		- payload size, 0 for auto
	RET (int):
		- renga_exec_code ret
*/
extern int renga_exec_code_for_user(const char *path, uint32_t csize);

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
extern int renga_work_commem_for_user(const char *path, uint32_t off, uint32_t csize, int mode);

/*
	set_logging_for_user(mode)
	Sets logging mode for user
	ARG 1 (int):
		- new logging mode
	RET (int):
		- renga_set_logging ret
*/
extern int renga_set_logging_for_user(int mode);

/*
	force_reset_framework(rexploit)
	Resets the commem and sets up the lv0 framework for user
	ARG 1 (int):
		- if 1, re-run the exploit
	RET (int):
		- renga_force_reset_framework ret
*/
extern int renga_force_reset_framework_for_user(int rexploit);

/*
	get_status_for_user(magic)
	Gets [magic] status for user
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- renga_get_status ret
*/
extern uint8_t renga_get_status_for_user(uint16_t magic);

/*
	get_opmode_for_user(magic)
	Gets [magic] status for user
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- renga_get_opmode ret
*/
extern uint8_t renga_get_opmode_for_user(uint16_t magic);

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
extern int renga_set_opmode_for_user(uint16_t magic, uint8_t opmode);

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
extern int renga_force_commemblock_for_user(int mode, int clean);

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
extern int renga_add_entry_for_user(char *path, uint8_t type, uint16_t magic);

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
extern int renga_remove_entry_for_user(uint8_t type, uint16_t magic);

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
extern int renga_xet_bank_for_user(int entry);