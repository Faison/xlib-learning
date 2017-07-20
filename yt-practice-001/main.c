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
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

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

  // Setup the hints for the Window Manager (Neeed for the window to actually show)
  XWMHints *wmhints = XAllocWMHints();
  // Specify that the WM Hints includes a value for the state.
  wmhints->flags = StateHint;
  // If we don't specify the initial state to be Normal, then it defaults to Withdrawn.
  wmhints->initial_state = NormalState;

  XSetWMHints(display, window, wmhints);

  // For some reason, the window isn't displaying unless we capture the Exposure event, so tell the x server we want to have this event.
  XSelectInput(display, window, ExposureMask);

  // Mapping the window to the display causes it to display.
  XMapWindow(display, window);

  // This variable will be used in processing events.
  XEvent e;

  // This function blocks until an Exposure event is thrown, and we don't want to do anything else until the window is exposed.
  XWindowEvent(display, window, ExposureMask, &e);

  // Use sleep to pause long enough for us to see the window.
  sleep(5);

  // Cleanup after ourselves.
  XFree(wmhints);
  wmhints = NULL;

  XDestroyWindow(display, window);
  XCloseDisplay(display);
  display = NULL;

  return EXIT_SUCCESS;
}
