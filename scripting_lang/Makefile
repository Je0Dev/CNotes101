# Makefile for our C-Script VM
CFLAGS = -Wall -Wextra -g -std=c99
OBJS = main.o chunk.o debug.o value.o lexer.o compiler.o vm.o

# Target executable
TARGET = cscript

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(TARGET) -lm

main.o: main.c chunk.h common.h debug.h vm.h
chunk.o: chunk.c chunk.h common.h value.h
debug.o: debug.c debug.h chunk.h
value.o: value.c value.h common.h
lexer.o: lexer.c lexer.h common.h
compiler.o: compiler.c compiler.h common.h lexer.h chunk.h value.h debug.h
vm.o: vm.c vm.h common.h chunk.h value.h compiler.h debug.h

clean:
	rm -f $(OBJS) $(TARGET)
