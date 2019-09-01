# Plaintext update_sm loader for 3.71 - 3.72 based on decsec-ldr
This tool will make lv0 load plaintext "ux0:data/update_sm.bin" instead of "os0:sm/update_sm.self".

# Usage
1) Download and install the VPK
2) Put decrypted update_sm elf in "ux0:data/update_sm.bin"
3) Run the app, it should say "module loaded"
	- If it hangs or crashes you will need to reboot by holding the POWER button
	
# Notes
 - Framework injection reliability: 80%
 - Framework stability: 90%
 - Before performing dangerous operations such as downgrading please make sure that the framework is stable (i.e by running the settings and welcome park apps)
 
# Credits
- Team Molecule for their crypto processor exploit and initial ~~spoonfeeding~~ help on discord.
- Mathieulh and d3s for their useful ideas.
- All the testers @henkaku & vita hacking discord servers