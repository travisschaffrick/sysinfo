CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
CLINK = -lncurses -ltinfo

sysinfo: menu.c sysinfo.c sysinfo.h
	$(CC) $(CFLAGS) $(CLINK) -o sysinfo menu.c sysinfo.c

clean:
	rm -f sysinfo
