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
	- For all communication ARM<->FRAMEWORK the Camera SRAM (p: 0x1C000000 - 0x1C1FE000) or a custom-mapped phycont block is used.
		- In renga it is referred to as "commem" or "corridor".
	- There are two patches used:
		- run_sm::set_state(5) hook - After SM load, before jumping to it.
		- fcmd_handler() hook - After ARM command is received, before executing it.
	- At every sleep/resume the crypto processor is reset, commem is reset too.
	- The framework is injected by exploiting update_sm::0x50002
	- If logging is enabled, psp2renga creates a log in "ux0:data/0psp2renga.log".

## Commem layout
	- 0x0-0x200: Framework config.
	- 0x200-0x500: Entry manager, run_sm hook jumps there.
	- 0x500-0xD0000: 12 0x10000 entry blocks, one additional smaller block.
	- 0xD0000-0xF0000: Reserved for any-time-run code, fcmd_handler hook jumps there.
	- 0xF0000-0x1FE000: Unused.

## Usage
	- For kernel-exports check /Include/renga-funcs.h
	- For user-exports check /Include/renga_user-funcs.h
	- For important defines check /Include/renga-defs.h
	- You may use lv0_loader.vpk (/User/app/) to easily run your own MeP payloads.
	- You may use renga.suprx (/User/plugins/) to easily switch between Camera SRAM and the phycont block.
		- Add the _settings one to SceSettings, there will be a new entry in Settings->System
		- Add the _forcephy or _forcesram to target apps, either phycont or sram mode will be forced.

# Credits
	- Team Molecule for the update_sm 0x50002 exploit and help over discord
	- Team Molecule for HenKaku, TaiHen and Enso
	- xerpi for the vita-baremetal-loader
	- CelesteBlue for the ssmgr-resume hook