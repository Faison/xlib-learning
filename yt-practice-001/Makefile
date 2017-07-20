#Basic makefile for compiling our application.

# -g Makes debug info available for tools like gdb.
# -Wall will make the compiler throw warnings for all things it can throw warnings for.
CFLAGS = -g -Wall

# pkg-config returns the info we need to compile an application that uses Xlib.
X11FLAGS = `pkg-config x11 --cflags --libs`

build:
	gcc main.c $(CFLAGS) $(X11FLAGS) -o main.o
