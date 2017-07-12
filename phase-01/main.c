/**
 * Phase 01 - Get a Window that works and can be closed.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <stdlib.h>
#include <string.h>
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

  // Grab a copy of X's representation of WM_PROTOCOLS, used in checking for window closed events.
  Atom wm_protocol = XInternAtom(dpy, "WM_PROTOCOLS", True);
  // Let the Window Manager know that we want the event when a user closes the window.
  Atom wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(dpy, win, &wm_delete, 1);

  // Map the window to the display.
  XMapWindow(dpy, win);

  XEvent e;

  // Block execution until the window is exposed.
  XWindowEvent(dpy, win, ExposureMask, &e);

  // After being exposed, we'll tell X what input events we want to know about here.
  XSelectInput(dpy, win, 0);

  // The loop
  // @TODO: Use sleeping to avoid taking up all CPU cycles.
  Bool done = False;

  while(!done) {
    // Handle events in the event queue.
    while(XPending(dpy) > 0) {
      XNextEvent(dpy, &e);
      switch(e.type) {
        case ClientMessage:
          // This client message is a window manager protocol.
          if (e.xclient.message_type == wm_protocol) {
            // Somehow this checks if the protocol was a WM_DELETE protocol, so we can exit the loop and be done.
            if (e.xclient.data.l[0] == wm_delete) {
              done = True;
            }
          }
          break;
      }
    }
    // Add the rest of the loop code here.
  }

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
