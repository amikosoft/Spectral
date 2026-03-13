extern Tigr *app;

unsigned gamepad(int);
unsigned gamepad3(int,float,float);

char *app_clipboard;

static int key_char[16] = {0}, key_chars = 0; // key_char = tigrReadChar(app);
#define key_char(idx) (key_char[(idx) % countof(key_char)])
#define key_read() do {\
    memset(key_char,0,sizeof(key_char)); \
    key_chars = 0; \
    do key_char[key_chars++] = tigrReadChar(app); while( key_chars < countof(key_char) && key_char[key_chars-1] ); \
} while(0)

int tigrKeyHeld_(Tigr *app, int vk) {
    assert(vk < 512);
    if( vk <= 16 ) return /*(gamepad(-1) & (1<<vk)) **/ (gamepad(1) & (1<<vk));
    return tigrKeyHeld(app, vk);
}
int tigrKeyDown_(Tigr *app, int vk) {
    assert(vk < 512);
    if( vk <= 16 ) return (gamepad(-1) & (1<<vk)) < (gamepad(1) & (1<<vk));
    return tigrKeyDown(app, vk);
}
int tigrKeyUp_(Tigr *app, int vk) {
    assert(vk < 512);
    if( vk <= 16 ) return (gamepad(-1) & (1<<vk)) > (gamepad(1) & (1<<vk));
    return tigrKeyUp(app, vk);
}

#define key_held(keycode) (!!(tigrKeyHeld_(app, keycode)))
#define key_down(keycode) (!!tigrKeyDown_(app, keycode))
#define key_trigger(keycode) (!!tigrKeyDown_(app, keycode)) // @todo: deprecate me, use key_down() instead
#define key_pressed(keycode) (!!(tigrKeyDown_(app, keycode) || tigrKeyHeld_(app, keycode)))
#define key_released(keycode) (!!tigrKeyUp_(app, keycode))

int key_repeat_(unsigned char vk, int table[256]) {
    table[vk] *= !!key_pressed(vk);
    table[vk] += !!key_pressed(vk);
    return table[vk] == 1 || table[vk] > 32;
}
int key_repeat(unsigned char vk) {
    static int table[256] = {0}; // @fixme: table[num_windows][256];
    return key_repeat_(vk, table);
}
int key_longpress(unsigned char vk) {
    static int table[256] = {0}; // @fixme: table[num_windows][256];
    table[vk] *= !!key_pressed(vk);
    table[vk] += !!key_pressed(vk);
    return table[vk] == 40; // 50==1.00s, 40==0.80s
}

char* keyboard() { // keys now[256] + old[256] + older[256] (2-frame delay)
    static char t[256*3] = {0};
    return t;
}
char* readkeys() { // reads whole keyboard. copies current into previous frame. updates current.
    char *kbd = keyboard();
    memcpy(kbd+512, kbd+256, 256);
    memcpy(kbd+256, kbd, 256);
    memcpy(kbd, tigrKeys(app), 256);
    return kbd;
}
