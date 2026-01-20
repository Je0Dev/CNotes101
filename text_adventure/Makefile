# Makefile for C/Lua Engine

# Find your Lua version. Try 'lua -v'
# This assumes 5.4. Adjust if you have 5.3, etc.
LUA_VER = 5.4

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -llua$(LUA_VER) -lm -ldl

all: game

game: engine.c gamedata.lua
	$(CC) $(CFLAGS) engine.c -o game $(LDFLAGS)

run: game
	./game

clean:
	rm -f game
