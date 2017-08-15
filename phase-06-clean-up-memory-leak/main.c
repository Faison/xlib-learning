/**
 * Phase 06 - Clean up the memory leak caused in the main window creation.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// ~16.6 ms between frames is ~60 fps.
#define RATE_LIMIT 16.6

#define _NET_WM_STATE_TOGGLE 2

// Forward declaration of this function so we can use it in main().
double timespec_diff(struct timespec *a, struct timespec *b);

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
   * and creating our own XTextProperty's.
   */
  // Sets Window properties that are used by the Window Manager.
  Xutf8SetWMProperties(dpy, win, "Phase 01", "", NULL, 0, sizehints, wmhints, NULL);

  // Tell X that we want to be notified of the Exposure event, so we can know when our window is initially visible.
  XSelectInput(dpy, win, ExposureMask);

  // Create a subwindow that has a white border.
  unsigned long white = WhitePixel(dpy, DefaultScreen(dpy));
  Window sub = XCreateSimpleWindow(dpy, win, 0, 0, 20, 20, 5, white, black);
  int sub_x = 0;
  int sub_y = 0;
  Bool moving_square = False;
  int move_offset_x = 0;
  int move_offset_y = 0;

  XMapWindow(dpy, sub);

  // Grab a copy of X's representation of WM_PROTOCOLS, used in checking for window closed events.
  Atom wm_protocol = XInternAtom(dpy, "WM_PROTOCOLS", True);
  // Let the Window Manager know that we want the event when a user closes the window.
  Atom wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(dpy, win, &wm_delete, 1);

  // Map the window to the display.
  XMapWindow(dpy, win);

  // This variable will be used to examine events thrown to our application window.
  XEvent e;

  // Block execution until the window is exposed.
  XWindowEvent(dpy, win, ExposureMask, &e);

  // After being exposed, we'll tell X what input events we want to know about here.
  XSelectInput(dpy, win, KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

  // The loop
  // @TODO: Use sleeping to avoid taking up all CPU cycles.
  Bool done = False;

  // We need to track very small periods of time (nanoseconds), so we use the struct timespec.
  struct timespec prev, curr;

  /*
   * Get the current time with CLOCK_MONOTONIC_RAW, which gets the time past since a certain time.
   * CLOCK_MONOTONIC_RAW is not subject to adjustments to the system clock.
   */
  clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
  // Initialize the previous time with the current time, that way our current vs. previous comparison is valid.
  prev.tv_sec = curr.tv_sec;
  prev.tv_nsec = curr.tv_nsec;

  // This variable will be used to normalize our loop to a specific rate.
  double mill_store = 0;

  // A couple of variables used to deal with KeyPress and KeyRelease events.
  KeySym event_key_0, event_key_1, lookup_keysym;
  char *key_0_string = NULL, *key_1_string = NULL, lookup_buffer[20];
  int lookup_buffer_size = 20, charcount = 0;
  Bool chatting = False;

  // windowed/fullscreen switching stuff.
  Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
  Atom fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
  XEvent window_change_event;
  memset(&window_change_event, 0, sizeof(window_change_event));
  window_change_event.type = ClientMessage;
  window_change_event.xclient.window = win;
  window_change_event.xclient.message_type = wm_state;
  window_change_event.xclient.format = 32;
  window_change_event.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
  window_change_event.xclient.data.l[1] = fullscreen;
  window_change_event.xclient.data.l[2] = 0;

  while(!done) {
    // Get the current time.
    clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
    // Store the difference in ms between curr and prev, store it in mill_store for use later.
    mill_store += timespec_diff(&curr, &prev);

    // @TODO: Determine if this should happen before updating curr.
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
        case KeyPress:
          /*
           * So there are two ways to deal with keypress events that I can find:
           *
           * 1. Use XLookupString to get the "string" value of the keypress. That will return the proper value
           *    when considering things like holding shift, caps lock enabled, numlock enabled, etc.
           *    It will not return a string value if you do a keypress combination that doesn't type a "character".
           *    This method probably works great for when you need a user to enter text.
           * 2. Use XLookupKeysym to get two Keysyms for index 0 and 1 (0 is normal click, 1 is shift or caps lock).
           *    Then, based on the key mask in e.xkey.state determine what was pressed (Like Ctrl + Shift + Up).
           *    This method wouldn't work well for when a user is entering text.
           *    This method probably works best for game controls.
           */
          // Handle KeyPress events.
          // @TODO: set the second value (index) properly
          charcount = XLookupString(&(e.xkey), lookup_buffer, lookup_buffer_size, &lookup_keysym, NULL);

          event_key_0 = XLookupKeysym(&(e.xkey), 0);
          event_key_1 = XLookupKeysym(&(e.xkey), 1);
          key_0_string = XKeysymToString(event_key_0);
          key_1_string = XKeysymToString(event_key_1);

          if (XK_Return == event_key_0) {
            if (chatting) {
              printf("\n-Done Chatting-\n");
              chatting = False;
            } else {
              printf("Message: \n");
              chatting = True;
            }
          } else if (event_key_0 == XK_Escape ) {
            done = True;
            printf("Pressed Escape, quitting.\n");
            continue;
          } else if (XK_q == event_key_0 && e.xkey.state & ControlMask) {
            done = True;
            printf("Pressed Ctrl+q, quitting.\n");
            continue;
          } else if (XK_F11 == event_key_0) {
            XSendEvent(dpy, DefaultRootWindow(dpy), False, SubstructureRedirectMask | SubstructureNotifyMask, &window_change_event);
            XFlush(dpy);
          }

          if (chatting) {
            printf("%s", lookup_buffer);
          } else {
            printf("Key pressed: %s - %s", key_0_string, key_1_string);
            if (e.xkey.state & ShiftMask) {
              printf(" | Shift");
            }
            if (e.xkey.state & LockMask) {
              printf(" | Lock");
            }
            if (e.xkey.state & ControlMask) {
              printf(" | Ctrl");
            }
            if (e.xkey.state & Mod1Mask) {
              printf(" | Alt");
            }
            if (e.xkey.state & Mod2Mask) {
              printf(" | Num Lock");
            }
            if (e.xkey.state & Mod3Mask) {
              printf(" | Mod3");
            }
            if (e.xkey.state & Mod4Mask) {
              printf(" | Mod4");
            }
            if (e.xkey.state & Mod5Mask) {
              printf(" | Mod5");
            }

            if (IsCursorKey(event_key_0)) {
              printf(" | Cursor Key (0)");
            }
            if (IsCursorKey(event_key_1)) {
              printf(" | Cursor Key (1)");
            }
            if (IsFunctionKey(event_key_0)) {
              printf(" | Function key (0)");
            }
            if (IsFunctionKey(event_key_1)) {
              printf(" | Function key (1)");
            }
            if (IsKeypadKey(event_key_0)) {
              printf(" | keypad (0)");
            }
            if (IsKeypadKey(event_key_1)) {
              printf(" | keypad (1)");
            }
            if (IsMiscFunctionKey(event_key_0)) {
              printf(" | Fn (0)");
            }
            if (IsMiscFunctionKey(event_key_1)) {
              printf(" | Fn (1)");
            }
            if (IsModifierKey(event_key_0)) {
              printf(" | Modifier (0)");
            }
            if (IsModifierKey(event_key_1)) {
              printf(" | Modifier (1)");
            }
            printf("\n");
          }
          break;
        case ButtonPress:
          printf("Button %d press at %d,%d\n", e.xbutton.button, e.xbutton.x, e.xbutton.y);

          if (e.xbutton.button == 1 && e.xbutton.x >= sub_x && e.xbutton.x < sub_x + 30 && e.xbutton.y >= sub_y && e.xbutton.y < sub_y + 30) {
            moving_square = True;
            move_offset_x = e.xbutton.x - sub_x;
            move_offset_y = e.xbutton.y - sub_y;
            printf("Clicked in the square!\n");
          }

          break;
        case ButtonRelease:
          printf("Button %d released at %d,%d\n", e.xbutton.button, e.xbutton.x, e.xbutton.y);

          if (e.xbutton.button == 1 && moving_square) {
            moving_square = False;

            sub_x = e.xbutton.x - move_offset_x;
            sub_y = e.xbutton.y - move_offset_y;

            move_offset_x = 0;
            move_offset_y = 0;
          }
          break;
        case MotionNotify:
          if (moving_square) {
            sub_x = e.xmotion.x - move_offset_x;
            sub_y = e.xmotion.y - move_offset_y;
          }
          break;
      }
    }

    // Only do stuff if the ms passed is greater than our rate limit.
    if (mill_store > RATE_LIMIT && !done) {
      /*
       * This loop counts down the mill_store, so if we have more ms stored than the Rate limit,
       * we run all the processes once. If we have three times the rate limit, we run all the
       * processes thrice. If the mill_store is less than the rate limit, then we pass on
       * processing for this time around the loop (which shouldn't really happen).
       *
       * This helps us have predictable numbers when we do things dependent on numbers, like physics.
       */
      for (; mill_store > RATE_LIMIT && ! done; mill_store -= RATE_LIMIT) {
        // Things that should run once per tick/frame will go here.
      }

      // Only render once per main loop, maybe?
      XMoveWindow(dpy, sub, sub_x, sub_y);
    }

    // Update the previous timespec with the most recent timespec so we can calculate the diff next time around.
    prev.tv_sec = curr.tv_sec;
    prev.tv_nsec = curr.tv_nsec;

    /**
     * Make our process sleep to avoid locking up the CPU.
     *
     * From what I understand, the following sleep code will not work on Windows.
     * It works on Linux, it probably works on OSX, but a different approach is needed
     * for Windows.
     */
    if (mill_store < RATE_LIMIT && !done) {
      // We'll need a couple of timespecs, and an int to check for errors.
      struct timespec sleep_required, sleep_remaining;
      int was_error = 0;

      // initialize the remaining sleep time with the value in mill_store.
      sleep_remaining.tv_sec = mill_store / 1000.0;
      sleep_remaining.tv_nsec = ((int)mill_store % 1000) * 1000000;

      do {
        // Set the required sleep time using the remaining time, so we can continue sleeping if nanosleep is interrupted.
        sleep_required.tv_sec = sleep_remaining.tv_sec;
        sleep_required.tv_nsec = sleep_remaining.tv_nsec;

        // Clear out the errno variable before calling nanosleep so we can catch errors.
        errno = 0;
        // Try sleeping for the required time, if nanosleep is interrupted, sleep_remaining will have the time left to sleep.
        was_error = nanosleep(&sleep_required, &sleep_remaining);
        // Keep looping if nanosleep was interrupted and there is some sleep time remaining.
      } while (was_error == -1 && errno == EINTR);
    }
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

/**
 * This returns the difference between the values of two timespecs.
 */
double timespec_diff(struct timespec *a, struct timespec *b)
{
	return (((a->tv_sec * 1000000000) + a->tv_nsec) - ((b->tv_sec * 1000000000) + b->tv_nsec)) / 1000000.0;
}
