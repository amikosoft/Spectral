// @todo: test all windowed/fullscreen scenarios

void tigrFocus(Tigr* bmp, bool focus) {
    if (!bmp || !bmp->handle) {
        return; // Invalid Tigr struct or bitmap
    }

    TigrInternal* win = tigrInternal(bmp);
    if (!win /*|| !win->shown*/) {
        return; // No TigrInternal or window not shown
    }

#ifdef _WIN32
    HWND hwnd = (HWND)bmp->handle;
    if (focus) {
        if (win->dwStyle & WS_POPUP) { // Borderless fullscreen
            ShowWindow(hwnd, SW_MAXIMIZE);
        } else {
            ShowWindow(hwnd, SW_RESTORE);
        }
        SetForegroundWindow(hwnd);
    } else {
        ShowWindow(hwnd, SW_MINIMIZE);
    }
#elif defined __APPLE__
    NSWindow* window = (NSWindow*)bmp->handle;
    if (focus) {
        if ([window isMiniaturized]) {
            [window deminiaturize:nil];
        }
        [window makeKeyAndOrderFront:nil];
        if ((win->flags & TIGR_FULLSCREEN) && !([window styleMask] & NSWindowStyleMaskFullScreen)) {
            [window toggleFullScreen:nil];
        }
    } else {
        [window miniaturize:nil];
    }
#elif defined __linux__
    Display* display = win->dpy;
    Window xwin = (Window)bmp->handle;
    if (!display || !xwin) {
        return; // No valid display or window
    }
    if (focus) {
        XMapRaised(display, xwin);
        XSetInputFocus(display, xwin, RevertToParent, CurrentTime);
        if (win->flags & TIGR_FULLSCREEN) {
            XEvent event = {0};
            event.type = ClientMessage;
            event.xclient.window = xwin;
            event.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", False);
            event.xclient.format = 32;
            event.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
            event.xclient.data.l[1] = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
            XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
        }
        XFlush(display);
    } else {
        XIconifyWindow(display, xwin, DefaultScreen(display));
        XFlush(display);
    }
#else
    return; // Unsupported platform
#endif
}

bool tigrFocused(Tigr* bmp) {
    if (!bmp || !bmp->handle) {
        return false; // Invalid Tigr struct or bitmap
    }

    TigrInternal* win = tigrInternal(bmp);
    if (!win /*|| !win->shown*/) {
        return false; // No TigrInternal or window not shown
    }

#ifdef _WIN32
    return GetFocus() == (HWND)bmp->handle;
#elif defined __APPLE__
    return [(NSWindow*)bmp->handle isKeyWindow];
#elif defined __linux__
    Display* display = win->dpy;
    Window xwin = (Window)bmp->handle;
    if (!display || !xwin) {
        return false; // No valid display or window
    }
    Window focus_win;
    int revert_to;
    XGetInputFocus(display, &focus_win, &revert_to);
    return focus_win == xwin;
#else
    return false; // Unsupported platform
#endif
}
