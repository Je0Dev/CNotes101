# Makefile for "Redis" Clone (C-Redis)

# Use C11 for atomics
CFLAGS = -Wall -Wextra -g -std=c11 -D_GNU_SOURCE

# Link pthreads and potentially atomics if needed (usually included)
LDFLAGS = -lpthread

# Object files
OBJS = main.o server.o slab.o lf_queue.o thread_pool.o hash_table.o

# Target executable
TARGET = c_redis

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

main.o: main.c server.h thread_pool.h slab.h
	gcc $(CFLAGS) -c main.c

server.o: server.c server.h common.h slab.h lf_queue.h thread_pool.h hash_table.h
	gcc $(CFLAGS) -c server.c

slab.o: slab.c slab.h common.h
	gcc $(CFLAGS) -c slab.c

lf_queue.o: lf_queue.c lf_queue.h common.h
	gcc $(CFLAGS) -c lf_queue.c

thread_pool.o: thread_pool.c thread_pool.h common.h lf_queue.h hash_table.h server.h
	gcc $(CFLAGS) -c thread_pool.c

hash_table.o: hash_table.c hash_table.h common.h
	gcc $(CFLAGS) -c hash_table.c

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	@echo "Starting C-Redis server on port 6379..."
	./$(TARGET)

