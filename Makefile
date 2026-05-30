CC = gcc
CFLAGS = $(shell pkg-config --cflags sdl3 sdl3-ttf)
LIBS = $(shell pkg-config --libs sdl3 sdl3-ttf)

snake_game: snake_game.c
	$(CC) snake_game.c -o snake_game $(CFLAGS) $(LIBS)

clean:
	rm -f snake_game
