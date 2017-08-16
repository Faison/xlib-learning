# Note, I really have no idea how most of this works, open to suggestions.
CFLAGS   = -std=gnu99 -g -Wall
X11FLAGS = `pkg-config x11 --cflags --libs`
GLFLAGS  = -lGL

build:
	gcc main.c $(CFLAGS) $(X11FLAGS) $(GLFLAGS) -o main.o
