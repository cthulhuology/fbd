fbd
===

Framebuffer Daemon

This program exposes a memory mapped file as an BGRA display surface so you and use it as a virtual /dev/fb0 for
Mac OS X and Windows.  Currently it is built using SDL2 http://libsdl.org and will one day support keyboards as well.


Why
---

I've been building a number of applications that interface with the operating system at a syscall level, but all of
the multimedia capabilities exist largely in userland libraries.  To make it possible to work around this stupid 
limitation (because modern operating system designs are crap), we fake a framebuffer device with a trivial 720p 
resolution and allow apps to write pixels to a memory region using a BGRA format.  

If hardware vendors didn't suck, and VESA were still a going concern, and OS designers not full of shit, this 
program would never exist.

