/*
	exec_code(cbuf, floc, csize, ctx)
	Copies [csize]/Reads [csize] from [cbuf] to (0x1C000000 + NMFEND_OFF + 0x100) and jumps to it there
	ARG 1 (void *):
		- source payload buf, set to NULL if source is a file
	ARG 2 (char *):
		- source payload file path, set to NULL if source is a buf
	ARG 3 (uint32_t):
		- payload size
	ARG 4 (int):
		- loaded SM context, set to 0 if no SM loaded
	RET (int):
		- NMFexec_code ret
*/
extern int renga_exec_code(void *cbuf, char *floc, uint32_t csize, int ctx);

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
extern int renga_add_entry(void *cbuf, char *file, uint32_t csize, uint8_t type, uint16_t magic);

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
extern int renga_remove_entry(uint8_t type, uint16_t magic);

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
extern int renga_set_opmode(uint16_t magic, uint8_t opmode);

/*
	get_opmode(magic)
	Gets operation mode for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- NMFget_opmode ret
*/
extern uint8_t renga_get_opmode(uint16_t magic);

/*
	get_status(magic)
	Gets status for [magic]
	ARG 1 (uint16_t):
		- entry magic (0x0 for the whole framework)
	RET (uint8_t):
		- NMFget_status ret
*/
extern uint8_t renga_get_status(uint16_t magic);

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
extern int renga_work_commem(void *buf, char *floc, uint32_t off, uint32_t size, int mode);

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
extern int renga_force_commemblock(int mode, int clean);

/*
	set_logging(mode)
	Sets logging mode
	ARG 1 (int):
		- new logging mode
	RET (int):
		- logging mode
*/
extern int renga_set_logging(int mode);

/*
	force_reset_framework(restore_mirror)
	Resets the commem and sets up the framework
	ARG 1 (int):
		- if 1, executes reset entries
	RET (int):
		- NMFsetup_framework ret
*/
extern int renga_force_reset_framework(int restore_mirror);

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
		- if func == NULL: current slot country
		- if func != NULL: func's slot
*/
extern int renga_add_reset_entry(void *vaddr, int cln_slot);
