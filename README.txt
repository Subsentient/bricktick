Bricktick is an curses based Arkanoid/Brickbreaker clone for UNIX.
It uses gmake so make sure you are using gmake if not on Linux.
It requires curses and that's about it really. Color is recommended.

0.9: Third release, adds Windows support through PDCurses and MinGW, fixes various warnings.

Compiling: Type make. If that fails, you can just link all the C files 
together and link them with curses.

For Windows, the only tested method is MXE cross compiler, which can be found at mxe.cc.
Example Windows build:
make CC=x86_64-w64-mingw32.static-gcc LDFLAGS=/home/whatever/mxe/x86_64-w64-mingw32.static/lib/pdcurses.a

Bugs: email subsentient@universe2.us or visit https://universe2.us.
