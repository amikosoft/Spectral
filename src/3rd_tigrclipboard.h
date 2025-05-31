char *tigrGetClipboard(Tigr *window); // must free() after use

#if 0
#include <stdlib.h>
#include <string.h>
#include "tigr.h"

#ifdef _WIN32
#include <windows.h>
#else
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#else
#ifdef __APPLE__
#include <objc/objc.h>
#include <objc/runtime.h>
#include <objc/message.h>
#endif
#endif
#endif
#endif

char *tigrGetClipboard(Tigr *window) { // must free() after use
    char *result = NULL;

#ifdef _WIN32
    // Windows implementation
    if (OpenClipboard(window->handle)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData) {
            WCHAR *wText = GlobalLock(hData);
            if (wText) {
                // Convert WCHAR (UTF-16) to UTF-8
                int len = WideCharToMultiByte(CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL);
                if (len > 0) {
                    result = (char *)malloc(len);
                    if (result) {
                        WideCharToMultiByte(CP_UTF8, 0, wText, -1, result, len, NULL, NULL);
                        // Clear the clipboard only if malloc succeeded
                        //EmptyClipboard();
                    }
                }
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }

#elif defined __linux__
    // Linux (X11) implementation
    TigrInternal* ti = tigrInternal(window);
    Display *display = ti->dpy;
    Window win = ti->win;
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom property = XInternAtom(display, "TIGR_CLIPBOARD", False);

    // Check if clipboard has an owner
    if (XGetSelectionOwner(display, clipboard) != None) {
        // Request clipboard data
        XConvertSelection(display, clipboard, utf8_string, property, win, CurrentTime);
        XSync(display, False);

        // Check for SelectionNotify event immediately
        XEvent event;
        if (XCheckTypedEvent(display, SelectionNotify, &event)) {
            if (event.xselection.property == property) {
                Atom type;
                int format;
                unsigned long len, bytes_left;
                unsigned char *data;
                XGetWindowProperty(display, win, property, 0, 0x1FFFFFFF, True,
                                  utf8_string, &type, &format, &len, &bytes_left, &data);
                if (data && len > 0) {
                    result = (char *)malloc(len + 1);
                    if (result) {
                        memcpy(result, data, len);
                        result[len] = '\0';
                        // Clear the clipboard only if malloc succeeded
                        //XSetSelectionOwner(display, clipboard, None, CurrentTime);
                        //XSync(display, False);
                    }
                    XFree(data);
                }
            }
        }
    }

#elif defined __APPLE__
    // macOS implementation using Objective-C runtime
    id nsStringClass = objc_getClass("NSString");
    id nsPasteboardClass = objc_getClass("NSPasteboard");

    id (*msgSend_id)(id, SEL) = (id (*)(id, SEL))objc_msgSend;
    id pasteboard = msgSend_id(nsPasteboardClass, sel_registerName("generalPasteboard"));
    
    id (*msgSend_stringWithUTF8String)(id, SEL, const char *) = (id (*)(id, SEL, const char *))objc_msgSend;
    id stringType = msgSend_stringWithUTF8String(nsStringClass, sel_registerName("stringWithUTF8String:"), "public.utf8-plain-text");

    id (*msgSend_stringForType)(id, SEL, id) = (id (*)(id, SEL, id))objc_msgSend;
    id string = msgSend_stringForType(pasteboard, sel_registerName("stringForType:"), stringType);

    if (string) {
        const char *(*msgSend_UTF8String)(id, SEL) = (const char *(*)(id, SEL))objc_msgSend;
        const char *utf8 = msgSend_UTF8String(string, sel_registerName("UTF8String"));
        if (utf8) {
            size_t len = strlen(utf8);
            result = (char *)malloc(len + 1);
            if (result) {
                strcpy(result, utf8);
                // Clear the clipboard only if malloc succeeded
                //objc_msgSend(pasteboard, sel_registerName("clearContents"));
            }
        }
    }
#endif

    return result;
}
