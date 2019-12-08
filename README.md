# psp2renga
Custom lv0 framework for Playstation Vita/TV

# Usage (end-user)
1) Put psp2renga.skprx in ur0:tai/
2) Add a line to ux0: or ur0: /tai/config.txt under *KERNEL
	- ur0:tai/psp2renga.skprx
3) Reboot

# For developers
psp2renga requires taihen.

## Basic info
	- For all communication ARM<->FRAMEWORK the Camera SRAM is used (p: 0x1C000000 - 0x1C1FE000).
		- In renga it is referred to as "commem" or "corridor".
	- There are two patches used:
		- run_sm::set_state(5) hook - After SM load, before jumping to it.
		- irq_handler::execute_handler(9) - After irq9 is triggered, before executing SM's handler.
	- At every sleep/resume the crypto processor is reset, commem is reset too.
	- The framework is injected by exploiting update_sm::0x50002
	- If logging is enabled, psp2renga creates a log in "ux0:data/0psp2renga.log".

## Commem layout
	- 0x0-0x200: Framework config.
	- 0x200-0x500: Entry manager, run_sm hook jumps there.
	- 0x500-0xD0000: 12 0x10000 entry blocks, one additional smaller block.
	- 0xD0000-0xF0000: Reserved for any-time-run code, irq_handler hook jumps there.
	- 0xF0000-0x1FE000: Unused.

## Usage
	- For kernel-exports check /Include/renga-funcs.h
	- For user-exports check /Include/renga_user-funcs.h
	- For important defines check /Include/renga-defs.h
	- You may use lv0_loader.vpk (/User/) to easily run your own MeP payloads.

# Credits
	- Team Molecule for the update_sm 0x50002 exploit and help over discord
	- Team Molecule for HenKaku, TaiHen and Enso
	- CelesteBlue for the ssmgr-resume hook