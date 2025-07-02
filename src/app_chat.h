// simple chat protocol that sits on top of irc protocol
// some design decisions:
// - we login with random nicks, so we dont get kicked if using already registered nicks in irc servers
// - we use client name as nick, parsed from the nick![~]client@addr mask.

#if 0
nicks seem to be 24-chars long or 15-chars long depending on server
texts seem to be 180-chars long approx
/ME text
/AWAY [text]
/TOPIC #channel text
/NICK newnick
/PART #channel
/JOIN #channel
/MODE #channel -l 20 ; limit 20users
/PRIVMSG [#channel|nick] text
/NOTICE  [#channel|nick] text
#endif

int   chat_users;
char* chat_lines[32];

int chat_maxlines() {
    // we adjust the number of chat lines dynamically, to avoid writing on the paper area
    // avail lines is roughly ((windowH - 192) / fontH) / 2
    int avail = ( ((_240-192)/2) / (theFontH + 2 + theFontPaddingH) ) - 1.5 /*ghosts counter*/;
    return tigrClamp(avail, 0, countof(chat_lines));
}
int chat_scroll() {
    if( chat_lines[0] ) {
        free(chat_lines[0]);

        for( int i = 1; i < countof(chat_lines); ++i) {
            chat_lines[i-1] = chat_lines[i];
        }

        chat_lines[countof(chat_lines)-1] = 0;
        return 1;
    }
    return 0;
}
int chat_queue(const char *nick, const char *text) {
    if( strchr(text, '~') ) { // colors
        char *t = va("%s", text);
        replace(t, "~1", "\1");
        replace(t, "~2", "\2");
        replace(t, "~3", "\3");
        replace(t, "~4", "\4");
        replace(t, "~5", "\5");
        replace(t, "~6", "\6");
        //replace(t, "~", "-"); // disable invert
        text = (const char*)t;
    }
    if( !strncmp(text, ":\1ACTION ", 9) ) { // this is a /me text
        text = va(":~%s", text+9);
    }
    for( int i = 0, end = chat_maxlines()/*countof(chat_lines)*/; i < end; ++i) {
        if( !chat_lines[i] ) {
            int color = (fnv1a(nick,strlen(nick)) % 6) + 1; // [1..6]
            chat_lines[i] = strdup(va("%c%s \7%s",color,nick,text));
            return 1;
        }
    }
    chat_scroll();
    chat_queue(nick, text);
    return 0;
}

int chat_printf(const char *fmt, ...) {
    static char *buf = 0; if(!buf) buf = REALLOC(0,64*1024);
    
    va_list vl;
    va_start(vl,fmt);
    int rc = vsnprintf(buf, 64*1024-1, fmt, vl);
    va_end(vl);

    if( DEV )
    printf("%s", buf);

    if( *buf == ':' ) {
        const char *who = buf; // may be server, nick or nick!addr
        const char *cmd = who ? strchr(who+1, ' ') : NULL; int wholen = cmd - who;
        const char *dst = cmd ? strchr(cmd+1, ' ') : NULL; int cmdlen = dst - cmd;
        const char *txt = dst ? strchr(dst+1, ' ') : NULL; int dstlen = txt - dst;
        const char *end = txt ?  txt + strlen(txt) : NULL; int txtlen = end - txt;

        if(who) ++who, --wholen;
        if(cmd) ++cmd, --cmdlen;
        if(dst) ++dst, --dstlen;
        if(txt) ++txt, --txtlen;

        if(who) printf("who: %.*s\n", wholen, who);
        if(cmd) printf("cmd: %.*s\n", cmdlen, cmd);
        if(dst) printf("dst: %.*s\n", dstlen, dst);
        if(txt) printf("txt: %.*s\n", txtlen, txt);

        if( cmd ) {
            // list users in channel
            if( atoi(cmd) == 353 ) {
                if( *txt == '=' ) chat_users = 0;
                chat_users += strcnt(strchr(txt, ':'), ' ') + 1;
                chat_users -= (strendi(txt, " \n") || strendi(txt, " "));
            } else {
                chat_users += !strncmp(cmd, "JOIN", 4) - !strncmp(cmd, "PART", 4) - !strncmp(cmd, "QUIT", 4);

                if( dst[0] == '#' && !strncmp(cmd, "PRIVMSG", 7) ) { // == m_ircd.chan
                    // split the nick!~client@addr mask
                    const char *client = strchr(who, '!')+1; client += *client == '~';
                    char nick[32]; snprintf(nick, 32, "%.*s", (int)(strchr(who, '@') - client), client);
                    chat_queue(nick, txt+1);
                }
            }
        }
    }

    return rc;
}

void chat_draw(Tigr *chat) {
    do_once {
        int chat_printf(const char *fmt, ...);
        ircd_printf = chat_printf;

        static char host[] = "127.0.0.1"; //"eu.dal.net"; // irc.dal.net eu.dal.net uk.eu.dal.net halcyon.dal.net
        static int port = 6667;

    #ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif

        srand(time(0));
        char randnum[24+1] = "zx";
        for(int i = strlen(randnum); i < 24; ++i) randnum[i] = '0' + (rand() % 10);

        strcpy(m_ircd.nick, randnum);
        strcpy(m_ircd.name, "Spectral" SPECTRAL); // seen in whois
        strcpy(m_ircd.client, ifdef(linux,getlogin(),ifdef(win32,getenv("USERNAME"),getenv("USER"))) );
        strcpy(m_ircd.chan, "#spectral112" ); // "#spectral" SPECTRAL);

        ircd_init(&m_ircd, host, port);
    }

    if( !ircd_update(&m_ircd) ) {
        alert("Cannot connect to multiplayer lobby server.\nPublic releases have no multiplayer support yet.");
        ZX_LOBBY = 0;
    }

    if( browser ) return;

    const char *chat_help = "- Click here or RSHIFT+ENTER to chat -";

    int say = 0, invite = 0;
    say |= key_held(TK_RSHIFT) && key_down(TK_RETURN);

    int chat_x = (_320-2*8)/2 - 8;
    int chat_y = (1-0.5) * 9;
    ui_at(chat,chat_x,chat_y);

    int mirrored = sin(time_ns() / 1e9) > 0;
    const char *ghosts[] = {"","",""}; // idle, walk1, walk2
    ui_mirror = mirrored;
    ui_x += !mirrored;
    say |= ui_click(chat_help, ghosts[(chat_users <= 1) ? 0 : 1]); // : 1 + !!ZXFlashFlag]);
    ui_x -= !mirrored;
    say |= ui_click(chat_help, "\f%d\n", chat_users);

    ui_at(chat,_320/6,ui_y);
    for( int i = 0; i < countof(chat_lines) && chat_lines[i]; ++i ) {
        ui_monospaced = 0;

        const char *who = chat_lines[i];
        const char *what = strchr(who, ' ')+1;
        int whatlen = strlen(what); whatlen -= what[whatlen-1] == '\n';
        invite |= ui_click(chat_help, "%.*s", (int)(what-1-who), who);
        ui_x += 8;
        say |= ui_click(chat_help, "%.*s\n", whatlen, what);
    }

    if( say ) {
        char *say = prompt("Chat","","");
        if( say[0] ) {
            chat_queue(m_ircd.client, say);
            servmsg(m_ircd.sock, "PRIVMSG", m_ircd.chan, say);
        }
        if( say[0] == '/' ) {
            char *cmd = say+1;
            const char *wat = strchr(cmd, ' '); if(wat) *(char*)wat = 0; wat = wat ? wat+1 : "";
            servmsg(m_ircd.sock, cmd, m_ircd.chan, wat);
        }
    }
    if( invite ) {
        ui_dialog_new("- Multiplayer -");
        ui_dialog_option(1,"Join \6user1\7's game\n", "Test A", 'JOIN',"16");
        ui_dialog_option(1,"Watch \6user1\7's game\n", "Test A", 'JOIN',"16");
#if 0
        ui_dialog_option(1,"\6User2\7 wants to join in. Allow? Y/n/N\n", "Test A", 'JOIN',"16");
        ui_dialog_option(1,"You are not allowed to join this game.\n", "Test A", 'JOIN',"16");

        ui_dialog_option(1,"You will shutdown current game. Sure?\n", "Test A", 'JOIN',"16");
#endif
        ui_dialog_separator();
        ui_dialog_option(1,"Return to lobby\n", "Test A", 'WIPE',va("%d", ZX));
        ui_dialog_cancel();
    }
}
