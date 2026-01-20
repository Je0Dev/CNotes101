# Makefile for mygit

# Link libcrypto (for SHA-1) and libz (for zlib)
LDFLAGS = -lcrypto -lz
CFLAGS = -Wall -Wextra -g

all: mygit

mygit: mygit.c
	gcc $(CFLAGS) mygit.c -o mygit $(LDFLAGS)

clean:
	rm -f mygit

