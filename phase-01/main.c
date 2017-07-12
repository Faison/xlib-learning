/**
 * Phase 01 - Get a Window that works and can be closed.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char *argv[])
{
  // Create application display.
  Display *dpy = XOpenDisplay(NULL);

  if (dpy == NULL) {
    return EXIT_FAILURE;
  }

  // Create the application Window.
  unsigned long black = BlackPixel(dpy, DefaultScreen(dpy));
  Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 800, 600, 0, black, black);

  // Setup the Window Manager hints.
  XWMHints *wmhints = XAllocWMHints();
  // This basically tells other functions that this contains a value for input and initial state.
  wmhints->flags = InputHint | StateHint;
  // And these are the values for input and initial state.
  wmhints->input = True;
  wmhints->initial_state = NormalState;

  // Setup the Size Hints (also for the Window Manager).
  XSizeHints *sizehints = XAllocSizeHints();
  // This tells other functions that the value for min width and height.
  sizehints->flags = PMinSize;
  // And these are the values for min width and height.
  sizehints->min_width = 400;
  sizehints->min_height = 300;

  /*
   * This particular function does some allocating that doesn't ever get freed.
   * Valgrind reports 27,262 bytes in 384 blocks as still reachable because of this.
   * It's possible that this "leak" could be avoided by using XSetWMProperties()
   * and create our own XTextProperty's.
   */
  // Sets Window properties that are used by the Window Manager.
  Xutf8SetWMProperties(dpy, win, "Phase 01", "", NULL, 0, sizehints, wmhints, NULL);

  // Tell X that we want to be notified of the Exposure event, so we can know when our window is initially visible.
  XSelectInput(dpy, win, ExposureMask);

  // Map the window to the display.
  XMapWindow(dpy, win);

  XEvent e;

  // Block execution until the window is exposed.
  XWindowEvent(dpy, win, ExposureMask, &e);

  // Free all the things.
  XFree(sizehints);
  sizehints = NULL;
  XFree(wmhints);
  wmhints = NULL;

  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
  dpy = NULL;

  return EXIT_SUCCESS;
}
