CC := gcc
CFLAGS = -Wall --std=c89
SRC = main.c wad.c 

compile:
	$(CC) $(CFLAGS) $(SRC)

run: compile
	copy MAP00.WAD.bak /B MAP00.WAD /Y
	.\a.exe "Hello world!" Bye world!

.DEFAULT_GOAL := run
