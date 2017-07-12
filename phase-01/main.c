/**
 * Phase 01 - Get a Window that works and can be closed.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[])
{
  Display *dpy = XOpenDisplay(NULL);

  if (dpy == NULL) {
    return EXIT_FAILURE;
  }

  XCloseDisplay(dpy);
  dpy = NULL;

  return EXIT_SUCCESS;
}
