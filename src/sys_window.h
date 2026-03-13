// Tigr flags:
// - TIGR_AUTO: enlarge allowed in any pixel ratio (_320,_240 vars may resize)
// - TIGR_FIXED: enlarge allowed in perfect integer pixel ratios (default) (_320,_240 vars do not resize)

extern Tigr *app;

#define window Tigr
#define window_flags(FS,ZOOM) (((FS) * TIGR_FULLSCREEN) | TIGR_AUTO/*TIGR_FIXED*/ | ((ZOOM)>4?TIGR_8X:(ZOOM)>=4?TIGR_4X:(ZOOM)>=3?TIGR_3X:(ZOOM)>=2?TIGR_2X:0))
#define window_closed() (tigrClosed(app))
#define window_close()  (app = (app ? tigrFree(app), NULL : NULL))
#define window_width()  (app->w)
#define window_height() (app->h)
#define window_title(title) tigrTitle(app,title)
void    window_override_icons();

char*   prompt(const char *title, const char *caption, const char *defaults );
#define alert(body) alert("Warning", body)
void    die(const char *msg);



#ifdef __APPLE__

char* prompt(const char *title, const char *body, const char *defaults ) {
    static char buffer[256]; buffer[0] = '\0';
    char *cmd = va("osascript -e 'text returned of (display dialog \"%s - %s\" default answer \"%s\")'", title, body, defaults);
    for( FILE *fp = popen(cmd, "r"); fp; pclose(fp), fp = 0 ) {
        if( fgets(buffer, 256, fp) >= 0 )
            while(strchr("\r\n", buffer[strlen(buffer)-1])) buffer[strlen(buffer)-1] = '\0';
    }
    puts(buffer);
    return buffer;
}
void window_override_icons() {
    
}

#elif defined _WIN32

#if 0
void ApplyDarkModeToControls(HWND hwnd, BOOL darkMode) {
    HWND hChild = GetWindow(hwnd, GW_CHILD);

    while (hChild) {
#       pragma comment(lib, "uxtheme")
        SetWindowTheme(hChild, darkMode ? L"DarkMode_Explorer" : NULL, NULL);

        ApplyDarkModeToControls(hChild, darkMode);
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
}
#endif

// Set the window icon for every window in your app (including MessageBox() calls and assertion failures) instead of just your primary window.
static HICON appIcon; // = (HICON)GetClassLong(hWnd, GCL_HICON);
static LRESULT WINAPI
window_create_callback(int type, WPARAM wparam, LPARAM lparam) {
    if (type == HCBT_CREATEWND) {
        SendMessage((HWND)wparam, WM_SETICON, ICON_SMALL, (LPARAM)appIcon);
        SendMessage((HWND)wparam, WM_SETICON, ICON_BIG, (LPARAM)appIcon);
        
        // does not work
        //ApplyDarkModeToControls((HWND)wparam, TRUE);
    }

#if 1 //< @r-lyeh dark mode: titlebar
    if (type == HCBT_ACTIVATE) {
        HWND hWnd = (HWND)wparam;
        {
            DWORD light_mode1 = 0;
            DWORD light_mode1_size = sizeof(light_mode1);
            LSTATUS result1 = RegGetValueW(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme",
                RRF_RT_REG_DWORD, NULL, &light_mode1, &light_mode1_size);

            DWORD light_mode2 = 0;
            DWORD light_mode2_size = sizeof(light_mode2);
            LSTATUS result2 = RegGetValueW(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme",
                RRF_RT_REG_DWORD, NULL, &light_mode2, &light_mode2_size);

            if( result1 == ERROR_SUCCESS || result2 == ERROR_SUCCESS ) {
                // dark titlebar
                enum DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
                DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
                BOOL enabled = (light_mode1 == 0 || light_mode2 == 0);

                DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(enabled));

                // dark controls. does not work. @fixme
                // if(enabled) ApplyDarkModeToControls(hWnd, TRUE);
            }
        }
    }
#endif

    return CallNextHookEx(NULL, type, wparam, lparam);
}
void window_override_icons() {
    do_once {
        HINSTANCE hInstance = (HINSTANCE)GetModuleHandleA(NULL);
        appIcon = ExtractIconA(hInstance, __argv[0], 0 );
        if(!appIcon) appIcon = ExtractIconA(hInstance, va("%s.exe", __argv[0]), 0 );
    }
    SetWindowsHookEx(WH_CBT, (HOOKPROC)window_create_callback, NULL, GetCurrentThreadId());
}
void window_replace_icons(HWND hwndMain, const char *pathfile_ico) {
    HICON hIcon32 = (HICON)LoadImageA(NULL, pathfile_ico, IMAGE_ICON, 32, 32, LR_LOADFROMFILE); // @leak
    HICON hIcon16 = (HICON)LoadImageA(NULL, pathfile_ico, IMAGE_ICON, 16, 16, LR_LOADFROMFILE); // @leak

    // Set on the window itself (title bar + taskbar)
    if(hIcon32) SendMessage(hwndMain, WM_SETICON, ICON_BIG,   (LPARAM)hIcon32);
    if(hIcon16) SendMessage(hwndMain, WM_SETICON, ICON_SMALL, (LPARAM)hIcon16);

    // Also update the class (helps with Alt+Tab consistency)
    if(hIcon32) SetClassLongPtr(hwndMain, GCLP_HICON,   (LONG_PTR)hIcon32);
    if(hIcon16) SetClassLongPtr(hwndMain, GCLP_HICONSM, (LONG_PTR)hIcon16);

    /**/ if(hIcon32) appIcon = hIcon32;
    else if(hIcon16) appIcon = hIcon16;
}
#else

char* prompt(const char *title, const char *body, const char *defaults ) {
    // order should be: kdialog, then zenity, then Xdialog
    // kdialog --title "title" --inputbox "" "body"
    // zenity --title "title" --entry --text "body"
    // Xdialog
    static char buffer[256]; buffer[0] = '\0';
    char *cmdk = va("kdialog --title \"%s\" --inputbox \"%s\" \"%s\"", title, body, defaults);
    char *cmdz = va("zenity --title \"%s\" --entry --text \"%s\" --entry-text \"%s\"", title, body, defaults);
    for( FILE *fp = popen(va("%s || %s", cmdk, cmdz), "r"); fp; pclose(fp), fp = 0 ) {
        if( fgets(buffer, 256, fp) >= 0 )
            while(strchr("\r\n", buffer[strlen(buffer)-1])) buffer[strlen(buffer)-1] = '\0';
    }
    puts(buffer);
    return buffer;
}

#define window_override_icons()

#endif

int (alert)(const char *title, const char *body) {
#ifdef _WIN32

    HWND hwndParent = GetActiveWindow(); // = GetForegroundWindow();

    bool is_fullscreen = 0;
    RECT r; is_fullscreen = GetWindowRect(hwndParent, &r) && !r.top && !r.left;

    if( is_fullscreen ) {
        // PostMessage(hwndParent, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        ShowWindow(hwndParent, SW_HIDE);
    }

    MessageBoxA(hwndParent, body, title, MB_OK|MB_SYSTEMMODAL);

    if( is_fullscreen ) {
        // PostMessage(hwndParent, WM_SYSCOMMAND, SC_RESTORE, 0);
        ShowWindow(hwndParent, SW_SHOW);
    }

//#elif is(ems)
//    emscripten_run_script(va("alert('%s')", body));
#elif defined __linux__ // is(linux)
    for(FILE *fp = fopen("/tmp/spectral.warning","wb");fp;fp=0)
    fputs(body,fp), fclose(fp), system("xmessage -center -file /tmp/spectral.warning");
#else // if is(osx)
    system(va("osascript -e 'display alert \"%s\" message \"%s\"'", title, body));
#endif
    return 1;
}

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    alert(msg);
#ifdef _WIN32
    if(IsDebuggerPresent()) DebugBreak();
#endif
    exit(-1);
}
