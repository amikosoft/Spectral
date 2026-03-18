#include "sys_xplat.h"

#include "sys_db.h"
#include "sys_file.h"
#include "sys_string.h"
#include "sys_sleep.h"
#include "sys_video.h"
#include "sys_window.h"
#include "sys_keyboard.h"
#include "sys_audio.h"
#include "sys_mouse.h"
#include "sys_gamepad.h"
#include "sys_ui.h"
#include "sys_ansi.h"
#include "sys_network.h"
#include "sys_embed.h"
#include "sys_ffmpeg.h"

#define VALNEW(v) v
#define VALDEL(v) 0
#include "sys_map.h"

#include "sys_zip.h"

static uint64_t splitmix64(uint64_t *x){
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
static uint64_t xoroshiro128(uint64_t s[2]) {
    uint64_t r = s[0] + s[1];
    s[1] ^= s[0];
    s[0] = (s[0]<<55 | s[0]>>9) ^ s[1] ^ (s[1]<<14);
    s[1] = (s[1]<<36 | s[1]>>28);
    return r;
}
static uint64_t rand64() {
    static uint64_t s[2] = {0};
    while( s[0] == 0 && s[1] == 0 ) {
        uint64_t z = (uint64_t)time(NULL);
        s[0] = splitmix64(&z);
        s[1] = splitmix64(&z);
    }
    return xoroshiro128(s);
}
