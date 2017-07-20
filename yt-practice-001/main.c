/**
 * Practice of the first video in the YouTubes.
 *
 * The goals of the first video are as follows:
 *   - Compile barebones C program using a Makefile.
 *   - Open a window, make it display on the screen for a few seconds (using sleep).
 *   - Make the window stay open until you click the close button.
 *   - Make sure to clean up after ourselves, shooting for 0 leaks.
 */

#include <stdlib.h>
#include <X11/Xlib.h>

int main(int argc, char **argv)
{
  // Open a connection to the X server.
  Display *display = XOpenDisplay(NULL);

  // Exit early if the display fails to open.
  if (display == NULL) {
    return EXIT_FAILURE;
  }

  // Grab the number for the black pixel for use in creating the window.
  unsigned long blackpixel = BlackPixel(display, DefaultScreen(display));
  // Creates the window, not currently mapped and displayed.
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 800, 600, 0, blackpixel, blackpixel);

  // Cleanup after ourselves.
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  display = NULL;

  return EXIT_SUCCESS;
}
