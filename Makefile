CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
CLINK = -lncurses -ltinfo

systats: menu.c systats.c systats.h
	$(CC) $(CFLAGS) $(CLINK) -o systats menu.c systats.c

clean:
	rm -rf pkg src sysinfo systats
	rm -f *.pkg.tar.zst *.tar.gz
	rm -f version.sh_no_phony.patch
	rm -f Makefilee systats.ce systats.he menu.ce
