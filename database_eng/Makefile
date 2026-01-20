# Makefile for B-Tree DB

CFLAGS = -Wall -Wextra -g -std=c99

all: btree_db

btree_db: main.o btree.o
	gcc $(CFLAGS) main.o btree.o -o btree_db

main.o: main.c btree.h
	gcc $(CFLAGS) -c main.c

btree.o: btree.c btree.h
	gcc $(CFLAGS) -c btree.c

clean:
	rm -f *.o btree_db my_database.db

