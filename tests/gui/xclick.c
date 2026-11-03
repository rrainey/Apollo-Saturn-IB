/* Sends a mouse press + release at window coordinates to the largest top-level
 * window on the display.  Used to drive the panel under Xvfb in the GUI test.
 * usage: xclick <x> <y> */
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void send(Display *d, Window w, int type, int x, int y) {
    XEvent e;
    memset(&e, 0, sizeof e);
    e.xbutton.type = type;
    e.xbutton.display = d;
    e.xbutton.window = w;
    e.xbutton.root = DefaultRootWindow(d);
    e.xbutton.x = x;
    e.xbutton.y = y;
    e.xbutton.button = Button1;
    e.xbutton.same_screen = True;
    XSendEvent(d, w, True, type == ButtonPress ? ButtonPressMask : ButtonReleaseMask, &e);
    XFlush(d);
}

int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "usage: xclick x y\n"); return 2; }
    Display *d = XOpenDisplay(NULL);
    if (!d) { fprintf(stderr, "xclick: cannot open display\n"); return 1; }
    Window root, parent, *kids = NULL, best = 0;
    unsigned int n = 0;
    long best_area = 0;
    XQueryTree(d, DefaultRootWindow(d), &root, &parent, &kids, &n);
    for (unsigned int i = 0; i < n; ++i) {
        XWindowAttributes a;
        if (XGetWindowAttributes(d, kids[i], &a) && a.map_state == IsViewable &&
            (long)a.width * a.height > best_area) {
            best_area = (long)a.width * a.height;
            best = kids[i];
        }
    }
    if (!best) { fprintf(stderr, "xclick: no window\n"); return 1; }
    send(d, best, ButtonPress, atoi(argv[1]), atoi(argv[2]));
    usleep(60000);
    send(d, best, ButtonRelease, atoi(argv[1]), atoi(argv[2]));
    XCloseDisplay(d);
    return 0;
}
