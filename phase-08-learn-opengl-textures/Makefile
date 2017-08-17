# Note, I really have no idea how most of this works, open to suggestions.
CFLAGS    = -std=gnu99 -g -Wall -lm
X11FLAGS  = `pkg-config x11 --cflags --libs`
GLFLAGS   = -lGL
SDL2FLAGS =  -lSDL2 -lSDL2_image

build:
	gcc main.c $(CFLAGS) $(X11FLAGS) $(GLFLAGS) $(SDL2FLAGS) -o main.o
