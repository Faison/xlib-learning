/**
 * Phase 01 - Get a Window that works and can be closed.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[])
{
  // Create the application window.
  Display *dpy = XOpenDisplay(NULL);

  if (dpy == NULL) {
    return EXIT_FAILURE;
  }

  unsigned long black = BlackPixel(dpy, DefaultScreen(dpy));
  Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 600, 800, 0, black, black);
  XMapWindow(dpy, win);

  // Free all the things.
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  dpy = NULL;

  return EXIT_SUCCESS;
}
