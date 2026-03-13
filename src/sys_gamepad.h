// St<<15|Bk<<14|RS<<13|LS<<12  // 4 extra buttons (thumbs+menus)
// RT<<11|LT<<10|RB<< 9|LB<< 8  // 4 extra buttons (shoulders)
//  Y<< 7| X<< 6| B<< 5| A<< 4  // 4 buttons
//  D<< 3| U<< 2| R<< 1| L<< 0; // d-pad

enum {
    PADVK_L  = 0,
    #define PAD_L (1 <<  0)
    PADVK_R  = 1,
    #define PAD_R (1 <<  1)
    PADVK_U  = 2,
    #define PAD_U (1 <<  2)
    PADVK_D  = 3,
    #define PAD_D (1 <<  3)
    PADVK_A  = 4,
    #define PAD_A (1 <<  4)
    PADVK_B  = 5,
    #define PAD_B (1 <<  5)
    PADVK_X  = 6,
    #define PAD_X (1 <<  6)
    PADVK_Y  = 7,
    #define PAD_Y (1 <<  7)
    PADVK_LB = 8,
    #define PAD_LB (1 <<  8)
    PADVK_RB = 9,
    #define PAD_RB (1 <<  9)
    PADVK_LT = 10,
    #define PAD_LT (1 << 10)
    PADVK_RT = 11,
    #define PAD_RT (1 << 11)
    PADVK_LS = 12,
    #define PAD_LS (1 << 12)
    PADVK_RS = 13,
    #define PAD_RS (1 << 13)
    PADVK_BK = 14,
    #define PAD_BK (1 << 14)
    PADVK_ST = 15,
    #define PAD_ST (1 << 15)
    PADVK_ON = 16,
    #define PAD_ON (1 << 16)
};

static
unsigned gamepad_update(GAMEPAD_DEVICE dev, float rumbleA, float rumbleB, float deadzoneX, float deadzoneY) {

    if (!GamepadIsConnected(dev)) {
        return 0;
    }

    GamepadSetRumble(dev, rumbleA, rumbleB); // 0.25f, 0.25f);

#if 0
    for(int i = 0; i < BUTTON_COUNT; ++i)
        printf("%d", GamepadButtonDown(dev, i));
    printf("\n");
#endif

    unsigned U = !!GamepadButtonDown(dev, BUTTON_DPAD_UP);
    unsigned D = !!GamepadButtonDown(dev, BUTTON_DPAD_DOWN);
    unsigned L = !!GamepadButtonDown(dev, BUTTON_DPAD_LEFT);
    unsigned R = !!GamepadButtonDown(dev, BUTTON_DPAD_RIGHT);

    unsigned A = !!GamepadButtonDown(dev, BUTTON_A);
    unsigned B = !!GamepadButtonDown(dev, BUTTON_B);
    unsigned X = !!GamepadButtonDown(dev, BUTTON_X);
    unsigned Y = !!GamepadButtonDown(dev, BUTTON_Y);

    unsigned Bk= !!GamepadButtonDown(dev, BUTTON_BACK);
    unsigned St= !!GamepadButtonDown(dev, BUTTON_START);

    unsigned LB = !!GamepadButtonDown(dev, BUTTON_LEFT_SHOULDER);
    unsigned RB = !!GamepadButtonDown(dev, BUTTON_RIGHT_SHOULDER);
    unsigned LS = !!GamepadButtonDown(dev, BUTTON_LEFT_THUMB);
    unsigned RS = !!GamepadButtonDown(dev, BUTTON_RIGHT_THUMB);

    float    LT_= GamepadTriggerLength(dev, TRIGGER_LEFT); 
    unsigned LT = LT_*LT_ >= (0.15f*0.15f); // @todo: deadzoneLT*deadzoneLT
    float    RT_= GamepadTriggerLength(dev, TRIGGER_RIGHT);
    unsigned RT = RT_*RT_ >= (0.15f*0.15f); // @todo: deadzoneRT*deadzoneRT

    float lx, ly, rx, ry;
    GamepadStickNormXY(dev, STICK_LEFT, &lx, &ly);
    GamepadStickNormXY(dev, STICK_RIGHT, &rx, &ry);

    if( (lx*lx) > (deadzoneX*deadzoneX) )
        if( lx < 0 ) L = 1;
        else
        if( lx > 0 ) R = 1;

    if( (ly*ly) > (deadzoneY*deadzoneY) )
        if( ly < 0 ) D = 1;
        else
        if( ly > 0 ) U = 1;

    if( deadzoneX < 0 ) { unsigned SWAP = L; L = R; R = SWAP; }
    if( deadzoneY < 0 ) { unsigned SWAP = D; D = U; U = SWAP; }

#if 0
        GamepadStickAngle(dev, STICK_LEFT);
        GamepadStickLength(dev, STICK_LEFT);
        GamepadStickAngle(dev, STICK_RIGHT);
        GamepadStickLength(dev, STICK_RIGHT);

                for (j = 0; j != BUTTON_COUNT; ++j) {
                    if (GamepadButtonTriggered(i, j)) {
                        printf("[%d] button triggered: %s", i, button_names[j]);
                    } else if (GamepadButtonReleased(i, j)) {
                        printf("[%d] button released:  %s", i, button_names[j]);
                    }
                }
                for (j = 0; j != TRIGGER_COUNT; ++j) {
                    if (GamepadTriggerTriggered(i, j)) {
                        printf("[%d] trigger pressed:  %d", i, j);
                    } else if (GamepadTriggerReleased(i, j)) {
                        printf("[%d] trigger released: %d", i, j);
                    }
                }
                for (j = 0; j != STICK_COUNT; ++j) {
                    for (k = 0; k != STICKDIR_COUNT; ++k) {
                        if (GamepadStickDirTriggered(i, j, k)) {
                            printf("[%d] stick direction:  %d -> %d", i, j, k);
                        }
                    }
                }
#endif

    return 1<<16
        | St<<15|Bk<<14|RS<<13|LS<<12  // 4 extra buttons (thumbs+menus)
        | RT<<11|LT<<10|RB<< 9|LB<< 8  // 4 extra buttons (shoulders)
        |  Y<< 7| X<< 6| B<< 5| A<< 4  // 4 buttons
        |  D<< 3| U<< 2| R<< 1| L<< 0; // d-pad
}


// gamepad id [1,2,3,4] -> status from current frame
// gamepad id [1,2,3,4] -> status from previous frame
// gamepad id [0] -> update controller status (during next frame)
unsigned gamepad3(int id, float deadzoneX, float deadzoneY) {
    extern Tigr *app;
    static void *init = 0;
    if( init != app->handle ) {
        if( init ) GamepadShutdown();
        GamepadInit(), init = app->handle;
    }

    static unsigned old[4] = {0};
    static unsigned pad[4] = {0};
    if( id == 0 ) {
        GamepadUpdate();
        old[0] = pad[0], pad[0] = gamepad_update(GAMEPAD_0+0, 0,0, deadzoneX,deadzoneY);
        old[1] = pad[1], pad[1] = gamepad_update(GAMEPAD_1+0, 0,0, deadzoneX,deadzoneY);
        old[2] = pad[2], pad[2] = gamepad_update(GAMEPAD_2+0, 0,0, deadzoneX,deadzoneY);
        old[3] = pad[3], pad[3] = gamepad_update(GAMEPAD_3+0, 0,0, deadzoneX,deadzoneY);
        return 0;
    }
    if( id > 0 ) return pad[(id-1) & 3];
    return old[(-id-1)&3];
}

unsigned gamepad(int id) {
    return gamepad3(id,0.15f,0.15f);
}
