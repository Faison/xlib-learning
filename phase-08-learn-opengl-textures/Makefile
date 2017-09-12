# Note, I really have no idea how most of this works, open to suggestions.
CFLAGS    = -std=gnu99 -g -Wall -lm
X11FLAGS  = `pkg-config x11 --cflags --libs`
GLFLAGS   = -lGL
SDL2FLAGS =  -lSDL2 -lSDL2_image

build: mat4
	gcc -o main.o main.c mat4.o $(CFLAGS) $(X11FLAGS) $(GLFLAGS) $(SDL2FLAGS)

mat4:
	gcc -c mat4.c -o mat4.o $(CFLAGS) $(GLFLAGS)
