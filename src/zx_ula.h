#define luma(r,g,b) ((byte)((r)*0.299+(g)*0.587+(b)*0.114))
#define gray(r,g,b) rgb(luma(r,g,b),luma(r,g,b),luma(r,g,b))

void draw_palette(Tigr *app, unsigned* pal16, const char *name) {
    int mx = mouse().x, my = mouse().y, lmb = mouse().lb && key_pressed(TK_SHIFT);
    int bottom = 1;
    int offy = bottom ? _240 - 16*2 : _240 - (_240-192)/2;

    for( int i = 0; i < 8; ++i ) {
        int offx = _320/2-(8*16)/2+i*16;
        tigrFill(app, offx,offy+00,16,16, (TPixel){.rgba = pal16[i]}),
        tigrFill(app, offx,offy+16,16,16, (TPixel){.rgba = pal16[i+8]});
        if( lmb && (my >= offy && my < offy+16) && (mx >= offx && mx < offx+16) ) {
            byte *entry = (byte*)(&pal16[i + 8 * (my >= _240-16)]);
            char *hexcolor = tinyfd_colorChooser(NULL,NULL,entry,entry); // #hex = pick(title,#hex,in,out)
        }
    }

    if( name ) {
        int dims = ui_print(NULL, 0,0, NULL, name);
        int w = dims & 0xFFFF;
        int h = dims >> 16; if( bottom && tape_playing() ) offy -= 24;
        ui_print(app, _320/2-w/2,offy+8+theFontH/2, ui_colors, name);
    }

    if( lmb ) {
        for(int i = 0; i < 16; ++i ) {
            TPixel p = (TPixel){.rgba = pal16[i]};
            printf("rgb(%3d,%3d,%3d),%s", p.r, p.g, p.b, !((i+1) % 4) ? "\n" : "" );
        }
    }
}

const char *ZXPaletteNames[] = {
    "Spectral",
    "Remix\n",
    "Gradients", // requires ZX_BLOOM=40
    "Atkinson", // probably better with ZX_BLOOM=20
    "Vivid",
    "Konni",
    "Sintez2\n",
    "Pico8",
    "Petit",
    "Bringer",
    "Fantasy",
    "Hue",
    "Dream",
    "Skin\n",
    "CPC",
    "EGA",
    "Gameboy",
    "PCW", // best with ZX_BLOOM=20
    "Amber", // best with ZX_BLOOM=90
    "Gray", // best with ZX_BLOOM=20
    "Negative\n", // best with ZX_BLOOM=20
    "External", // must be last entry
};

rgba ZXPalettes[][64] = {

    // two sections each, 16 regular colors in total. 64 entries for ulaplus, though
    // normal: black,blue,red,pink,green,cyan,yellow,white
    // bright: black,blue,red,pink,green,cyan,yellow,white

    {
        // spectral palette. note: no pure black
        // use 0xAB if used with bloom; 0xC0 otherwise
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xC0),rgb(0xC0,0x00,0x00),rgb(0xC0,0x00,0xC0),
        rgb(0x00,0xC0,0x00),rgb(0x00,0xC0,0xC0),rgb(0xC0,0xC0,0x00),rgb(0xC0,0xC0,0xC0),
        rgb(0x12,0x10,0x12),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
    },
    {   // remix, v1
        // based on fantasy15+dream15+hue15+skin5+sintez2
        rgb(  0,  0,  0),rgb(  0, 33,133),rgb(191, 34,  0),rgb(162,  0,166),
        rgb(  0,160,  0),rgb( 10,143,247),rgb(255,160,  0),rgb(200,190,180), // 0,180,230  0,159,193 // 0,118,215
        rgb(  0,  0,  0),rgb(  0, 30,221),rgb(250, 16,  0),rgb(220,  0, 84),
        rgb(  0,227,  0),rgb(  0,242,175),rgb(255,220,  0),rgb(240,240,255),
    },
    {
        // Gradients (from specemu; probably chev's), needs to be used with gloom so it burns the colors and increases luma
        rgb(000,000,000),rgb(000,000,143),rgb(143,000,000),rgb(143,000,143),
        rgb(000,143,000),rgb(000,143,143),rgb(143,143,000),rgb(143,143,143),
        rgb(000,000,000),rgb(000,000,255),rgb(217,000,000),rgb(217,000,255),
        rgb(000,217,000),rgb(000,217,255),rgb(217,217,000),rgb(217,217,255),
    },
    {
        // Richard Atkinson's colors (zx16/48/zx+ only)
        rgb(0x06,0x08,0x00),rgb(0x0D,0x13,0xA7),rgb(0xBD,0x07,0x07),rgb(0xC3,0x12,0xAF),
        rgb(0x07,0xBA,0x0C),rgb(0x0D,0xC6,0xB4),rgb(0xBC,0xB9,0x14),rgb(0xC2,0xC4,0xBC),
        rgb(0x06,0x08,0x00),rgb(0x16,0x1C,0xB0),rgb(0xCE,0x18,0x18),rgb(0xDC,0x2C,0xC8),
        rgb(0x28,0xDC,0x2D),rgb(0x36,0xEF,0xDE),rgb(0xEE,0xEB,0x46),rgb(0xFD,0xFF,0xF7),
    },
    {
        // Vivid: what most pc emulators use
        // note: C0->D7 seems fine too
        // note: D8->FF is another option. see @Polyducks: "A true-to-hardware palette. Please note that the luminesence of the ZX Spectrum is dictated by the voltage output of the hardware (85% voltage for non-bright, 100% for bright) - so instead of using E/F values as dictated in the wikipedia article in the hex for non-bright/bright, the colours are instead D8/FF (D8 being 85% of FF). For example, non-bright red is given as EE0000 in the article - instead it has been portrayed here as D80000 to give as close to hardware output as possible on modern screens."
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xC0),rgb(0xC0,0x00,0x00),rgb(0xC0,0x00,0xC0),
        rgb(0x00,0xC0,0x00),rgb(0x00,0xC0,0xC0),rgb(0xC0,0xC0,0x00),rgb(0xC0,0xC0,0xC0),
        rgb(0x00,0x00,0x00),rgb(0x00,0x00,0xFF),rgb(0xFF,0x00,0x00),rgb(0xFF,0x00,0xFF),
        rgb(0x00,0xFF,0x00),rgb(0x00,0xFF,0xFF),rgb(0xFF,0xFF,0x00),rgb(0xFF,0xFF,0xFF),
    },
    {
        // jussi ala-konni's
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x28*4),rgb(0x30*4,0x00*4,0x00*4),rgb(0x30*4,0x00*4,0x28*4),rgb(0x00*4,0x2c*4,0x00*4),rgb(0x00*4,0x2c*4,0x28*4),rgb(0x30*4,0x2c*4,0x00*4),rgb(0x30*4,0x2c*4,0x28*4),
        rgb(0x00*4,0x00*4,0x00*4),rgb(0x00*4,0x00*4,0x37*4),rgb(0x3f*4,0x00*4,0x00*4),rgb(0x3f*4,0x00*4,0x37*4),rgb(0x00*4,0x3b*4,0x00*4),rgb(0x00*4,0x3b*4,0x37*4),rgb(0x3f*4,0x3b*4,0x00*4),rgb(0x3f*4,0x3b*4,0x37*4),
    },
    {   // sampled from signal sintez 2 capture at https://www.sinclaircollection.site/?page_id=484
        // then adjusted levels, lumas and hues. then major corrections
        rgb(  4,  0, 14),rgb( 12,  3,173),rgb(208, 24, 31),rgb(175, 32,241),
        rgb(  2,145, 81),rgb( 10,143,247),rgb(186,182, 12),rgb(182,179,198),
        rgb(  4,  0, 14),rgb( 13, 44,251),rgb(251, 40, 24),rgb(255, 64,230),
        rgb(  2,210,103),rgb( 10,219,255),rgb(225,225,  0),rgb(213,217,234),
    },
    {
        // pico8: bright blue had to be added. dark gray dropped
        rgb(000,000,000),rgb( 29, 43, 83),rgb(171, 82, 54),rgb(126, 37, 83),
        rgb(000,135, 81),rgb(131,118,156),rgb(255,163,000),rgb(252,202,168),
        rgb(000,000,000),rgb( 29, 43,155),rgb(255,000, 77),rgb(255,119,168),
        rgb(000,228, 54),rgb( 41,173,255),rgb(255,236, 39),rgb(255,241,232)
    },
    {
        // petit: adapted from https://lospec.com/palette-list/petit-computer
        // darkgray as blue1, brown as red1. patched gray into cyan5. dropped skintone
        rgb(  0,  0,  0),rgb( 56, 56, 56),rgb(146, 89, 40),rgb(121, 56,251),
        rgb(  0,121,  0),rgb(  0,140,140),rgb(251,162,  0),rgb(186,186,186),
        rgb(  0,  0,  0),rgb(  0, 56,243),rgb(251, 24,  0),rgb(251, 89,195),
        rgb(  0,243, 24),rgb(  0,186,251),rgb(251,227,  0),rgb(251,251,251),
    },
    {
        // dawnbringer's original
        //rgb( 20, 12, 28),rgb( 48, 52,109),rgb(133, 76, 48),rgb( 68, 36, 52),
        //rgb( 52,101, 36),rgb(133,149,161),rgb(210,125, 44),rgb(117,113, 97),
        //rgb( 20, 12, 28),rgb( 89,125,206),rgb(208, 70, 72),rgb(210,170,153),
        //rgb(109,170, 44),rgb(109,194,202),rgb(218,212, 94),rgb(222,238,214),
        // dawnbringer-ish, saturated by 175%, then tweaked blue1,pink3,gray7 to fit the ZX mood
        // then adjusted manually per entry
        rgb( 20, 12, 28),rgb( 40, 57,151),rgb(180, 31, 14),rgb(186, 39, 98),
        rgb( 49,160, 16),rgb(100,175,189),rgb(222,177,  1),rgb(205,201,190),
        rgb( 20, 12, 28),rgb( 33, 33,255),rgb(255, 33, 33),rgb(233, 33,126),
        rgb( 89,199,  0),rgb( 68,216,224),rgb(234,234,  0),rgb(232,252,190),
    },
    {
        // fantasy15, v1
        // rgb(0x04,0x0c,0x18),rgb(0x00,0x00,0xc0),rgb(0xc0,0x00,0x00),rgb(0x9d,0x00,0xd9),
        // rgb(0x00,0xc0,0x00),rgb(0x00,0xc0,0xc0),rgb(0xc8,0xc8,0x00),rgb(0xc0,0xc0,0xc0),
        // rgb(0x04,0x0c,0x18),rgb(0x00,0x40,0xff),rgb(0xea,0x06,0x40),rgb(0xff,0x40,0xff),
        // rgb(0x40,0xff,0x00),rgb(0x04,0xff,0xa2),rgb(0xff,0xd8,0x00),rgb(0xff,0xff,0xff),
        // fantasy15, v2
        rgb(  4, 12, 24),rgb( 12, 24,180),rgb(192, 11,  0),rgb(202,  0,106),
        rgb(  7,184, 20),rgb(  0,191,213),rgb(255,155,  0),rgb(211,199,154),
        rgb(  4, 12, 24),rgb( 67, 11,159),rgb(255, 16,  0),rgb(255,  0, 77),
        rgb( 86,230,  0),rgb( 56,252,179),rgb(255,230,  0),rgb(255,255,187),
    },
    {   // hue15, v1
        rgb(  0,  0,  0),rgb(  0, 33,133),rgb(150, 24,  0),rgb(184, 22,143),
        rgb(  0,176,  0),rgb(  0,183,173),rgb(255,170,  0),rgb(200,190,180),
        rgb(  0,  0,  0),rgb(  0,  0,204),rgb(204,  0,  0),rgb(226,  0, 94),
        rgb( 50,223,  0),rgb(  4,255,144),rgb(255,230,  0),rgb(240,240,255),
    },
    {
        // dream15, v1
        // rgb(  4, 12, 24),rgb( 11, 73,125),rgb(191, 57, 34),rgb(114, 63,182),
        // rgb(  0,191,115),rgb( 53, 98,253),rgb(255,128, 66),rgb(211,199,154),
        // rgb(  4, 12, 24),rgb( 41, 46,199),rgb(203,  1, 36),rgb(171, 52, 79),
        // rgb(255,230,  0),rgb(  0,186,251),rgb(250,204,112),rgb(255,255,170),
        //rgb(  4, 12, 24),rgb( 10, 64,131),rgb(204,  0,  0),rgb(176,  0,176),
        //rgb( 57,230,  0),rgb(  0, 98,244),rgb(255,128, 66),rgb(216,196,169),
        //rgb(  4, 12, 24),rgb( 28, 33,255),rgb(240,  0, 61),rgb(202,  0,106), //193, 49, 78),
        //rgb(223,255,  0),rgb(  0,255,255),rgb(250,204,112),rgb(255,255,170),
        // rgb(  4, 12, 24),rgb( 36, 61, 89),rgb(204,  0,  0),rgb( 67, 11,159),
        // rgb( 74,203, 31),rgb(  0, 98,244),rgb(255,136, 17),rgb(216,196,169),
        // rgb(  4, 12, 24),rgb(  0, 33,255),rgb(255,  0, 40),rgb(202,  0,106),
        // rgb(222,240,  0),rgb(  0,255,255),rgb(250,204,112),rgb(255,255,170),
        //rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(200, 55,146),
        //rgb(  0,191, 63),rgb( 57,130,249),rgb(255,120, 55),rgb(216,196,169), //green: rgb(  0,128, 64)
        //rgb(  0,  0,  0),rgb( 79, 45,125),rgb(255, 40,  3),rgb(234,  0, 88),
        //rgb( 13,233,  1),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
        // rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(177,  7,148),
        // rgb(  0,144,  0),rgb( 57,130,249),rgb(255,120, 55),rgb(216,196,169), // yellow: rgb(255,112, 43)
        // rgb(  0,  0,  0),rgb( 79, 45,125),rgb(255, 40,  3),rgb(228,  5,134),
        // rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150), // yellow: rgb(250,190, 67)
        //rgb(  0,  0,  0),rgb( 31, 69,140),rgb(204,  0,  0),rgb(79,  45,125),
        //rgb(  0,144,  0),rgb( 57,130,249),rgb(255,112, 43),rgb(216,196,169),
        //rgb(  0,  0,  0),rgb(  0,  0,255),rgb(255, 40,  3),rgb(177,  7,148),
        //rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
        rgb(  0,  0,  0),rgb(  0, 64,159),rgb(204,  0,  0),rgb(196,  4,217),
        rgb(  0,144,  0),rgb( 57,130,249),rgb(255,136, 17),rgb(216,196,169),
        rgb(  0,  0,  0),rgb(  0, 80,255),rgb(255, 40,  3),rgb(254, 10,125),
        rgb( 64,192,  0),rgb(  3,218,191),rgb(250,188, 58),rgb(255,255,150),
    },
    {   // skin5, v1
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 40,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  7,141,156),rgb(230,155,  2),rgb(210,182,130),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(255, 26,  0),rgb(219, 17, 72),
        //rgb(204,233,  1),rgb( 27,252,168),rgb(247,209, 81),rgb(255,228,185),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  7,141,156),rgb(230,155,  2),rgb(210,182,130),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(219,  0, 72),
        // rgb(  0,204,  0),rgb(  3,233,147),rgb(247,209, 81),rgb(255,228,185),
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  7,141,156),rgb(227,128,  0),rgb(254,203,133),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(255,  4, 92),
        //rgb(  0,204,  0),rgb(  3,233,147),rgb(255,174, 94),rgb(255,255,187),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  7,141,156),rgb(227,128,  0),rgb(216,151, 92),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(255,  4, 92),
        // rgb(  0,204,  0),rgb(  3,233,147),rgb(255,174, 94),rgb(255,255,150),
        //rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        //rgb(  0,147,  0),rgb(  0,118,215),rgb(227,159,  0),rgb(200,146,106),
        //rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        //rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174, 94),rgb(221,221,204),
        // rgb(  0,  0,  0),rgb( 54,  0,136),rgb(170, 50,  0),rgb(137,  0,140),
        // rgb(  0,147,  0),rgb(  0,118,215),rgb(227,159,  0),rgb(216,176,148),
        // rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        // rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174, 94),rgb(255,255,204),
        rgb(  0,  0,  0),rgb( 54,  0,136),rgb(157, 35,  0),rgb(137,  0,140),
        rgb(  0,147,  0),rgb(  0,118,215),rgb(207,122, 37),rgb(203,159,112),
        rgb(  0,  0,  0),rgb( 32,  0,210),rgb(204,  0,  0),rgb(225,  0, 79),
        rgb(  0,204,  0),rgb(  0,183,221),rgb(255,174,120),rgb(255,245,204),
    },
    {
        // adapted from amstrad cpc - https://www.cpcwiki.eu/index.php/CPC_Palette
        // https://www.grimware.org/doku.php/documentations/devices/gatearray
        rgbm(0x00,0x02,0x01,1.00),rgbm(0x05,0x06,0x63,1.00),rgbm(0x6C,0x02,0x01,1.00),rgbm(0x69,0x02,0x68,1.00),
        rgbm(0x02,0x78,0x01,1.00),rgbm(0x00,0x78,0x68,1.00),rgbm(0x6E,0x7B,0x01,1.00),rgbm(0x6E,0x7D,0x6B,1.00),
        rgbm(0x00,0x02,0x01,1.00),rgbm(0x0C,0x02,0xF4,0.75),rgbm(0xF3,0x05,0x06,0.75),rgbm(0xF3,0x02,0xF4,0.75),
        rgbm(0x71,0xF5,0x04,0.75),rgbm(0x71,0xF3,0xF4,0.75),rgbm(0xF3,0xF3,0x0D,0.75),rgbm(0xFF,0xF3,0xF9,0.75),
    },
    {
        // adapted from EGA64 palette - https://commons.wikimedia.org/wiki/File:EGA64_Full_Palette.png
        rgb(  0,  0,  0),rgb(  0,  0,170),rgb(170,  0,  0),rgb(170,  0, 85),
        rgb(  0,170, 85),rgb(  0,170,255),rgb(170,170,  0),rgb(170,170,170),
        rgb(  0,  0,  0),rgb(  0,  0,255),rgb(255,  0,  0),rgb(255,  0, 85),
        rgb(  0,255,  0),rgb(  0,255,255),rgb(255,255,  0),rgb(255,255,255),
    },
    {
        // gameboy. should be 4 shades, but we're doing 8 shades: many games are unplayable otherwise.
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
        rgb( 35-10, 84-10, 28-10),rgb( 35, 84, 28),rgb( 59-10,158-10, 59-10),rgb( 59,158, 59),rgb(124-10,186-10, 49-10),rgb(124,186, 49),rgb(174-10,255-10, 38-10),rgb(174,255, 38),
    },
    {
        // pcw-ish. take pc emulators > b/w version > green. limited to 8 lumas for better vis (should be 4!)
        rgb(0,luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100), // boost G+B
        rgb(0,luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100),
        rgb(0,luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100),
        rgb(0,luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100),
        rgb(0,luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100),
        rgb(0,luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100),
        rgb(0,luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100),

        rgb(0,luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100), // there is no black in a pcw monitor afaik. use a dark green instead
        rgb(0,luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100), // boost G+B
        rgb(0,luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100),
        rgb(0,luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100),
        rgb(0,luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100),
        rgb(0,luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100),
        rgb(0,luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100),
        rgb(0,luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100),
    },
    {
        // amber-ish. take pc emulators > b/w version > orange. limited to 8 lumas for better vis
        rgb(luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100,0), // boost G+B
        rgb(luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100,0),
        rgb(luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100,0),
        rgb(luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100,0),
        rgb(luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100,0),
        rgb(luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100,0),
        rgb(luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100,0),

        rgb(luma(0x00,0x20,0x00),luma(0x00,0x20,0x00)*44/100,0), // there is no black in an amber monitor afaik. use a dark orange instead
        rgb(luma(0x00,0x20,0xEA),luma(0x00,0x20,0xEA)*44/100,0), // boost G+B
        rgb(luma(0xD0,0x00,0x00),luma(0xD0,0x00,0x00)*44/100,0),
        rgb(luma(0xD0,0x00,0xD0),luma(0xD0,0x00,0xD0)*44/100,0),
        rgb(luma(0x00,0xD0,0x00),luma(0x00,0xD0,0x00)*44/100,0),
        rgb(luma(0x00,0xD0,0xD0),luma(0x00,0xD0,0xD0)*44/100,0),
        rgb(luma(0xD0,0xD0,0x00),luma(0xD0,0xD0,0x00)*44/100,0),
        rgb(luma(0xD0,0xD0,0xD0),luma(0xD0,0xD0,0xD0)*44/100,0),
    },
    {
        // pc emulators, b/w version
        gray(0x00,0x00,0x00),gray(0x00,0x00,0xC0),gray(0xC0,0x00,0x00),gray(0xC0,0x00,0xC0),
        gray(0x00,0xC0,0x00),gray(0x00,0xC0,0xC0),gray(0xC0,0xC0,0x00),gray(0xC0,0xC0,0xC0),
        gray(0x00,0x00,0x00),gray(0x00,0x00,0xFF),gray(0xFF,0x00,0x00),gray(0xFF,0x00,0xFF),
        gray(0x00,0xFF,0x00),gray(0x00,0xFF,0xFF),gray(0xFF,0xFF,0x00),gray(0xFF,0xFF,0xFF),
    },
    {
        // pc emulators, b/w version, tv with inverted y+c signal. ^=0xFF >> 00->FF,FF->00,C0->63
        gray(255,255,255),gray(255,255, 63),gray( 63,255,255),gray( 63,255, 63),
        gray(255, 63,255),gray(255, 63, 63),gray( 63, 63,255),gray( 63, 63, 63),
        gray(255,255,255),gray(255,255,000),gray(000,255,255),gray(000,255,000),
        gray(255,000,255),gray(255,000,000),gray(000,000,255),gray(000,000,000),
    },
    {
        // external. must be last entry
    },
};

void palette_use(int palette) {
    memcpy(ZXPalette, ZXPalettes[palette], sizeof(rgba) * 64);
}

int pal_loadbin(const void *pal32, int size) {
    if( pal32 && size == 64 ) {
        if( writefile(".Spectral/Spectral.pal", pal32, size) ) { // save for later
            memcpy(ZXPalettes[countof(ZXPalettes) - 1], pal32, 64);
            return 1;
        }
    }
    return 0;
}
int pal_load(const char *fname) {
    int size; char *data = readfile(fname, &size);
    int rc = pal_loadbin(data, size);
    if( data ) free(data);
    return rc;
}

// contention: http://www.zxdesign.info/memContRevision.shtml
// @todo: dotcrawl (16/48) https://spectrumforeveryone.co.uk/technical/spectrum-compatibility-issues/
// @fixme: "on Issue 1 and 2 machines that do not have the ‘spider mod’ installed, the behaviour of the ULA is to contend all I/O access. This is important, as we’ll see later."

    // @todo: floating bus https://spectrumforeveryone.com/technical/memory-contention-floating-bus/
    // @todo: floating bus https://spectrumforeveryone.com/technical/memory-contention-floating-bus/
    //
    // floating bus [16,48,128,+2]
    //
    // [ref] https://sinclair.wiki.zxnet.co.uk/wiki/Floating_bus
    // [ref] https://github.com/jsmolina/z88dk-tutorial-sp1/blob/master/floating-bus.md
    // [ref] https://softspectrum48.weebly.com/notes/category/floating-bus
    //
    // note: for +2A/+3 models, it works this way:
    // While the effect is no longer present on unused ports, it is still evident when reading from ports
    // which match a particular addressing pattern, expressed as (1+n*4), or in binary, 0000 xxxx xxxx xx01.
    // This is only evident when the memory paging ports are unlocked, otherwise they will always return FF.
    // In detail:
    // 1. It is only found on ports that follow the pattern (1 + (4 * n) && n < 0x1000) (that is, ports 1, 5, 9, 13 . . . 4093).
    // 2. The bus always returns 0xFF if bit 5 of port 32765 is set (i.e. paging is disabled), so it won’t work in 48K mode.
    // 3. Otherwise, the value returned is the value currently read by the ULA ORed with 1 (i.e. Bit 0 is always set).
    // 4. During idling intervals (i.e. when the ULA is drawing the border or in between fetching the bitmap/attribute byte), the bus latches onto the last value that was written to, or read from, contended memory, and not strictly 0xFF. This is crucial to keep in mind.
    //
    // @fixme: for floatspy.tap to be stable
    // IM2 t_offs needs to be 25->22 (128K), 29->30 (48K)

byte ulaplus_mode = 0; // 0:pal,1:mode,else:undefined
byte ulaplus_data = 0;
byte ulaplus_enabled = 0;
byte ulaplus_grayscale = 0;
byte ulaplus_registers[64+1] = {0};

void ula_reset() {
    ulaplus_mode = 0;
    ulaplus_data = 0;
    ulaplus_enabled = 0;
    ulaplus_grayscale = 0;
    memset(ulaplus_registers, 0, sizeof(ulaplus_registers));
}

uint64_t transact(uint64_t pins);

void draw_8_pixels(rgba *texture, const unsigned *palette, rgba *begin, rgba *end, byte pixel, byte attr) {
    rgba fg, bg;

    if (ulaplus_enabled) {
        fg = palette[((attr & 0xc0) >> 2) | ((attr & 0x07))];
        bg = palette[((attr & 0xc0) >> 2) | ((attr & 0x38) >> 3) | 8];
    } else {
        fg = palette[((attr >> 3) & 0x08) | (attr & 0x07)];
        bg = palette[((attr >> 3) & 0x0F)];
        pixel ^= (attr & 0x80) && ZXFlashFlag ? 0xff : 0x00;
    }

    // @fixme: make section branchless

    if( /*(texture + 0) >= begin &&*/ (texture + 0) < end ) texture[0] = pixel & 0x80 ? fg : bg;
    if( /*(texture + 1) >= begin &&*/ (texture + 1) < end ) texture[1] = pixel & 0x40 ? fg : bg;
    if( /*(texture + 2) >= begin &&*/ (texture + 2) < end ) texture[2] = pixel & 0x20 ? fg : bg;
    if( /*(texture + 3) >= begin &&*/ (texture + 3) < end ) texture[3] = pixel & 0x10 ? fg : bg;
    if( /*(texture + 4) >= begin &&*/ (texture + 4) < end ) texture[4] = pixel & 0x08 ? fg : bg;
    if( /*(texture + 5) >= begin &&*/ (texture + 5) < end ) texture[5] = pixel & 0x04 ? fg : bg;
    if( /*(texture + 6) >= begin &&*/ (texture + 6) < end ) texture[6] = pixel & 0x02 ? fg : bg;
    if( /*(texture + 7) >= begin &&*/ (texture + 7) < end ) texture[7] = pixel & 0x01 ? fg : bg;
}

void run(int do_sim, int do_int, int TS, int x, int y, TPixel *pix) {
    if(do_sim) zx_int = do_int;
    if(TS<=0) { printf("error in scanline, TS=%d\n", TS); return; }

    extern Tigr* canvas;
    rgba *texture = pix && y >= 0 && y < _192 ? (rgba*)&pix[x + y * _256] : (rgba*)NULL;
    rgba *begin = (rgba*)&canvas->pix[0];
    rgba *end = (rgba*)&canvas->pix[_255 + _191 * _256];

    uint64_t epoch = ticks;
    int is_paper = TS == 128;
    int is_contended = is_paper;

    int CENTER_Y = _24 + (ZX_PENTAGON ? 8 : 0);

#if 0 // DEV
    // ultrawide ula+ uses default zx palette for border
    const unsigned *palette = !is_paper && ZX_ULAPLUS > 1 ? ZXPalettes[0] : ZXPalette;
#else
    const unsigned *palette = ZXPalette;
#endif

    int pixels = -1, attribs = -1;
    if( is_paper ) {
        if( y >= CENTER_Y && y < (CENTER_Y+192) ) {
            y -= CENTER_Y;

            // int third = y / 64; int y64 = y % 64;
            // int bit3swap = (y64 & 0x38) >> 3 | (y64 & 0x07) << 3;
            // int scanline = (bit3swap + third * 64) << 5;
            #define SCANLINE(y) \
                ((((((y)%64) & 0x38) >> 3 | (((y)%64) & 0x07) << 3) + ((y)/64) * 64) << 5)

            assert(VRAM);
            assert(y >= 0 && y < 192);
            pixels=SCANLINE(y);
            attribs=6144+((y&0xF8)<<2);
        }

        if( ZX_PENTAGON ) is_contended = 0; // disable further contention
        if( ZX_TURBOROM && tape_feeding() ) is_contended = 0;

        #ifdef DEBUG_SCANLINE
        //memset32(texture - x, ~0u, _256);
        //sys_sleep(1000/60.);
        #endif

        // RF: misalignment. also glitches faster at max cpu speed
        if( ZX_RF && texture ) {
            enum { BAD = 8, POOR = 32, DECENT = 256 };
            int shift0 = (rand()<(RAND_MAX/(ZX_FASTCPU?2:POOR))); // flick_frame * -(!!((y+0)&0x18))
            *texture = ZXPalette[ZXBorderColor];
            texture += shift0;
        }
    }
    if( DEV && ZX_DEVTOOLS && key_pressed(TK_TAB) ) pixels = attribs = -1;

    int P = 0; // paper tick
    static int B = 0; // border tick // static uint64_t B = 0;
    static byte fetch[4] = {0}; // pixel 0x4000, attrib 0x5800, pixel 0x4001, attrib 0x5801, IDLE, IDLE, IDLE, IDLE ...

    while( TS-- ) {

        // see: https://sinclair.wiki.zxnet.co.uk/wiki/Floating_bus
        if( is_paper ) {
            /**/ if(P == 3-3)   *(floating_bus = fetch+0) = pixels >= 0 ? VRAM[pixels++] : 0xFF;
            else if(P == 4-3)   *(floating_bus = fetch+1) = attribs >= 0 ? VRAM[attribs++] : ZXBorderColor;
            else if(P == 5-3)   *(floating_bus = fetch+2) = pixels >= 0 ? VRAM[pixels++] : 0xFF;
            else if(P == 6-3) { *(floating_bus = fetch+3) = attribs >= 0 ? VRAM[attribs++] : ZXBorderColor;
                // Output 16 pixels every 8 cycles
                if( texture )
                    draw_8_pixels( (texture += 8) - 8, palette, begin, end, fetch[0], fetch[1]),
                    draw_8_pixels( (texture += 8) - 8, palette, begin, end, fetch[2], fetch[3]);
            }
            else floating_bus = NULL;
            P = (P+1) & 7;
        } else {
            floating_bus = NULL;
            if( (B++/*ticks*/ & 3) == 1 )
                if( texture )
                    draw_8_pixels( (texture += 8) - 8, palette, begin, end, 0xFF, ZXBorderColor); // | 64);
        }

        if( !do_sim ) continue;

        ++ticks;
        sys_audio();
        fdc_tick(1);

        // paper contention (border has no contention)
        if( is_contended ) {
            // see: https://sinclair.wiki.zxnet.co.uk/wiki/Contended_memory
            // see: https://sinclair.wiki.zxnet.co.uk/wiki/Contended_I/O
            // see: http://www.zxdesign.info/memContRevision.shtml
            unsigned Ts = (ticks - epoch) & 7;
            unsigned Ys = ZX > 200 ? Ts != 1 : Ts < 6; // either 1N765432 pattern for +2A/+3 case, or 654321NN pattern for 48/128/+2. 
            if( Ys ) { // this happens on the first tstate (T1) of any instruction fetch, memory read or memory write operation.
                const uint16_t addr = Z80_GET_ADDR(pins);

                if( ZX >= 210 ? pins & Z80_MREQ : pins & (Z80_MREQ|Z80_IORQ) || !floating_bus ) {
                    if( pins & Z80_IORQ ) // (Z80_M1|Z80_MREQ|Z80_IORQ|Z80_RD|Z80_WR|Z80_RFSH);
                    if( (addr & 0x0001) == 0x0000 ) continue;
                    if( (addr & 0xC000) == 0x4000 ) continue; // equivalent to (addr >= 0x4000 && addr < 0x8000)
                    if( (addr & 0xC000) == 0xC000 ) { // Contention is also applied if the address is between 0xc000 and 0xffff on a 128K Spectrum with a contended RAM bank paged into that address range. Note: 48K always has bank 0 paged in, which is not contended
                        // 16/48/128/+2: pages 1,3,5,7 are contended (1), 0,2,4,6 not contended (0) -> so mask is 0001 (1)
                        // +2A/+3:       pages 4,5,6,7 are contended (1), 0,1,2,3 not contended (0) -> so mask is 0100 (4)
                        unsigned bank = page128&7; // (unsigned)(MEMr[addr >> 14] - mem) / 0x4000;
                        unsigned is_contended_bank = (bank & (ZX >= 210 ? 4 : 1));
                        if( is_contended_bank ) continue;
                    }
                }

                // I/O operations performed during T3 and T4 cycles.
                // Same than memory but it adds extra checks: contention of paged-in bank and A0 low bit.
                //
                // A15-A14 in contended page | A0 Low bit | Contention T3 pattern | Contention T4 pattern
                //        No                 |      0     |  N                    |  YNNN
                //        No                 |      1     |  N                    |  NNN
                //        Yes                |      0     |  YN                   |  YNNN
                //        Yes                |      1     |  YN                   |  YN, YN, YN

                // detects t3 and t4
                // uint64_t b4 = pins & (Z80_M1|Z80_WAIT|Z80_IORQ|Z80_RD|Z80_MREQ);
                // uint64_t b3 = pins & (Z80_M1|Z80_WAIT|Z80_IORQ|Z80_RD|Z80_MREQ|Z80_WR|Z80_RFSH);
                // uint64_t is_t4 = (b4 == 0) || (b4 == Z80_MREQ && (pins & Z80_WR));
                // uint64_t is_t3 = !(b3 & (Z80_M1|Z80_RFSH)) && ((b3 & Z80_WAIT) || ((b3 & (Z80_MREQ|Z80_IORQ)) && (b3 & (Z80_RD|Z80_WR))));
            }
        }

        tape_ticks += !!mic_on; // tick tape after contention, otherwise turborom will break

        // clear INT pin after 32/36 ticks
        if (int_counter > 0) {
            if(!--int_counter) z80_interrupt(&cpu, 0);
        }

        if (zx_int /*  && cpu.step == 2 && IFF1(cpu) */ ) { // adjust
            zx_int = 0;

            // request vblank interrupt
            z80_interrupt(&cpu, 1);
            B = 0;

            // hold the INT pin for 32/36 ticks (or 37 if late timings)
            // most models start in late_timings, and convert into early_timings as they heat up
            // however, +2 is always late_timings.
            int late = 1;
            int_counter = 32 + (4+late) * (ZX == 128 || ZX == 200 || ZX_PENTAGON);

            RZX_tick();
        }
        
        pins = z80_tick(&cpu, pins);
        pins = transact(pins);
    }
}



#define CLAMP(v, minv, maxv) ((v) < (minv) ? (minv) : (v) > (maxv) ? (maxv) : (v))
#define REMAP(var, src_min, src_max, dst_min, dst_max) \
    (dst_min + ((CLAMP(var, src_min, src_max) - src_min) / (float)(src_max - src_min)) * (dst_max - dst_min))

static float dt;
static double timer;
static int flick_frame;
static int flick_hz;

void blur(window *win) {
    int height = _24+192+_24;
    int width = _32+256+_32;

    // screen
    static int j = 0;
    static byte jj = 0;

    // RF noise interferences
#if 1
    enum { _33 = 31 };
#else
    static int _33 = 31;
    int diff = key_down(TK_RIGHT) - key_down(TK_LEFT);
    if( diff ) printf("%d\n", _33 = tigrClamp(_33+diff, 1, 180));
#endif

    for( int y = _24; y < _192-_24; ++y ) {
        rgba *texture = &((rgba*)win->pix)[_32 + y * width];

        for(int x=0;x<((_256-_32*2)/8);x++) {

        int shift = (++jj)&1;
        texture += shift;

                // RF: hue shift
                // @fixme: apply this effect to the upper and bottom border as well
                //         restricted to paper area only for perf reasons. note that very few games 
                //         would write noticeable graphics in border area (eg, Sentinel48, DefendersOfTheEarth.dsk))
                for(int i = 8-1; i >= 0; --i) {
                    unsigned pix0 = texture[i-0];
                    unsigned pix1 = texture[i-1];
                    unsigned pix2 = texture[i-2];
                    unsigned r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    unsigned r1,g1,b1; rgb_split(pix1,r1,g1,b1);
                    unsigned r2,g2,b2; rgb_split(pix2,r2,g2,b2);
                    if(i&1)
                    texture[i] = rgb((r0+r0+r0+r0)/4,(g0+g0+g0+g0)/4,(b2+b2+b2)/3); // yellow left
                    else
                    texture[i] = rgb((r0+r0+r0+r1)/4,(g0+g0+g0+g1)/4,(b1+b1+b1+b2)/4); // blue left
                    //else
                    //texture[i] = rgb((r0+r1+r1+r1)/4,(g0+g1+g1+g1)/4,(b0+b1+b1)/3);

                    continue;
if(ZX_RF) {
                    // saturate aberrations (very slow)
                    pix0 = texture[i];
                    rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    if( s0 * 1.5 > 255 ) s0 = 255; else s0 *= 1.5;
                    if( v0 * 1.01 > 255 ) v0 = 255; else v0 *= 1.01;
                    texture[i] = as_rgb(h0,s0,v0);
}
                }

                // RF: jailbars (paper area only)
                if(x%2)
                for(int i = 0; i < 8; ++i) {
                    unsigned pix0 = texture[i-0];
                    byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    texture[i] = as_rgb(h0,s0,v0*0.99);
                }

                // RF: noise interferences
                // interesting tv effects (j): 13, 19, 23, 27, 29, 33, etc.
                // note: original RF effect (j==33) created a pattern that was way more
                // visible in 320x240 resolution with no CRT (bilinear smoothing) at all.
                // used to be 33 all the time, then 13 (since CRT), now 31 (since 384x304).
                // @fixme: apply this effect to the upper and bottom border as well
                //         restricted to paper area only for perf reasons.
                for(int i = 0; i < 8; ++i) { ++j; j%=_33;
                    unsigned pix0 = texture[i-0];
                    byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
                    byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
                    if(j<1) texture[i] = as_rgb(h0,s0,v0*0.95);
                    else
                    if(j<2) texture[i] = as_rgb(h0,s0,v0*0.95);
                    else
                            texture[i] = rgb(r0,g0,b0);
                }

        texture -= shift;

            texture += 8;
        }
    }
}

void scanlines(window *win) {
    // hsv: slow. may be better in a shader
    int height = _24+192+_24;
    int width = _32+256+_32;
    for(int y = 0; y < _192; y+=2) {
        rgba *texture = &((rgba*)win->pix)[0 + y * width];
        for(int x = 0; x < width; ++x) {
            unsigned pix0 = texture[x];
            byte r0,g0,b0; rgb_split(pix0,r0,g0,b0);
            byte h0,s0,v0; rgb2hsv(r0,g0,b0,&h0,&s0,&v0);
            texture[x] = as_rgb(h0*0.99,s0,v0*0.98);
        }
    }
}

void frame(int drawmode, int do_sim) { // no render (<0), whole frame (0), scanlines (1)
    extern window *canvas;

    // notify new frame
    if(do_sim) frame_new();

    // zx_int = 1;
    //
    // games that are sensitive to INT placement: 
    // 48: aticatac(game), sidewize(game), parapshock(menu), arkanoid(game), 
    // 128: blacklamp128(game) (I is set to 0x4000-0x7fff range for a couple of frames, might be that?)
    // +3: bonanzabros.dsk(menu), 
    // ANY: floatspy.tap, overscan.tap, 

    // drawmode < 0 (no render), == 0 (frame based), > 0 (scanline based)
    TPixel *pix = drawmode < 0 ? NULL : canvas->pix;

    // timing constants YY(vertical scanline), XX(horizontal T), PP(horizontal T for Pentagon), BR(border)
    #define ENUM(...) ifdef(DEV,static int, enum{) __VA_ARGS__ ifndef(DEV,}); ifndef(DEV,, \
        if( ZX_DEVTOOLS ) { \
        if( key_repeat('A') ) printf("ADD:%d XX:%d BR:%d\n", ADD = (ADD+1) % 228, XX, BR); \
        if( key_repeat('X') ) printf("ADD:%d XX:%d BR:%d\n", ADD, XX = (XX+1) % (228), BR); \
        if( key_repeat('B') ) printf("ADD:%d XX:%d BR:%d\n", ADD, XX, BR = (BR+1) % ((228-128)/2)); \
        } );

    // tests used:
    // debugbreak[48/128/Plus3/Pentagon].z80 to test the scanline Y/INT placement
    // megashock[48/128].z80 to test the frame length (69888/70908)
    // bonanzabros.dsk wont work if INT is not placed in scanlineY >= 3

    // SCANLINE RENDER
    // ensure INT code is at end of frame
    if( ZX_PENTAGON ) {
        // Pentagon: see https://worldofspectrum.net/rusfaq/index.html
        // 320 scanlines = 16 vsync + 64  upper + 192 paper + 48 bottom
        // each scanline = 32 hsync + 36 border + 128 paper + 28 border = 224 TS/scanline
        // total = (16+64+192+48) * 224 = 320 * (32+36+128+28) = 320 * 224 = 71680 TS

#if 1
        enum { _32 = _24 + 8 }; // _24 + OFFSET_Y };

        ENUM( TS = 224, XX = 188, BR = 32, ADD = 0 );
        run(do_sim,1,TS-XX,0,-1,NULL);
        for( int y = 0; y <   80; ++y ) run(do_sim,0,TS,0,y-(80-_32),pix);
        for( int y = 0; y <  192; ++y ) run(do_sim,0,BR,0,_32+y,pix),run(do_sim,0,128,BR*2,_32+y,pix),run(do_sim,0,TS-128-BR,(128+BR)*2,_32+y,pix);
        for( int y = 0; y < 48-1; ++y ) run(do_sim,0,TS,0,_32+192+y,pix);
        run(do_sim,0,XX,0,-1,NULL);
#else
        const int TS = 224, BR = 32;
        for( int y = 0; y <  16; ++y ) run(do_sim,!y,TS,0,0,NULL);
        for( int y = 0; y <  64; ++y ) run(do_sim,0,TS,0, y-(64-_24),pix);
        for( int y = 0; y < 192; ++y ) run(do_sim,0,BR,0, _24+y,pix),run(do_sim,0,128,BR*2, _24+y,pix),run(do_sim,0,TS-128-BR,(128+BR)*2, _24+y,pix);
        for( int y = 0; y <  48; ++y ) run(do_sim,0,TS,0, _24+192+y,pix);
#endif

        //< @fixme: ADD here could be maybe 10Ts, according to 48k/128k cases. not sure.
    }
    else if( ZX < 128 ) {
        // 48K: see https://wiki.speccy.org/cursos/ensamblador/interrupciones http://www.zxdesign.info/interrupts.shtml
        // 312 scanlines = 16 vsync + 48  upper + 192 paper + 56 bottom
        // each scanline = 48 hsync + 24 border + 128 paper + 24 border = 224 TS/scanline
        // total = (16+48+192+56) * 224 = 312 * (48+24+128+24) = 312 * 224 = 69888 TS

#if 1
        ENUM( TS = 224, XX = 28/**27*/, BR = 32, ADD = 4 );
        run(do_sim,1,TS-XX,0,-1,NULL);
        for( int y = 1; y <   64; ++y ) run(do_sim,0,TS,0,y-(64-_24),pix);
        for( int y = 0; y <  192; ++y ) run(do_sim,0,BR,0,_24+y,pix),run(do_sim,0,128,BR*2,_24+y,pix),run(do_sim,0,TS-128-BR,(128+BR)*2,_24+y,pix);
        for( int y = 0; y <   56; ++y ) run(do_sim,0,TS,0,_24+192+y,pix);
        run(do_sim,0,XX+ADD,0,-1,NULL);
#else
        const int TS = 224, BR = 32, early_timings = 1;
        run(do_sim,0,24-early_timings,0,-1,NULL), run(do_sim,1,TS-24-early_timings,0,-1,NULL);
        for( int y = 1; y <  64; ++y ) run(do_sim,0,TS,0,y-(64-_24), pix);
        for( int y = 0; y < 192; ++y ) run(do_sim,0,BR,0,_24+y, pix),run(do_sim,0,128,BR*2,_24+y, pix),run(do_sim,0,TS-128-BR,(128+BR)*2,_24+y, pix);
        for( int y = 0; y <  56; ++y ) run(do_sim,0,TS,0,_24+192+y, pix);
#endif

        // @fixme: ADD should be 0
        // ADD(3) fixes sidewize. why?
        // ADD(4) fixes borderbreak48. why?

    } else {
        // 128K:https://wiki.speccy.org/cursos/ensamblador/interrupciones https://zx-pk.ru/threads/7720-higgins-spectrum-emulator/page4.html
        // 311 scanlines = 15 vsync + 48  upper + 192 paper + 56 bottom
        // each scanline = 48 hsync + 26 border + 128 paper + 26 border = 228 TS/scanline
        // total = (15+48+192+56) * 228 = 311 * (48+26+128+26) = 311 * 228 = 70908 TS

#if 1
        ENUM( TS = 228, XX = 27, BR = 32, ADD = 2 );
        run(do_sim,1,TS-XX,0,-1,NULL);
        for( int y = 1; y <   63; ++y ) run(do_sim,0,TS,0,y-(63-_24),pix);
        for( int y = 0; y <  192; ++y ) run(do_sim,0,BR,0,_24+y,pix),run(do_sim,0,128,BR*2,_24+y,pix),run(do_sim,0,TS-128-BR,(128+BR)*2,_24+y,pix);
        for( int y = 0; y <   56; ++y ) run(do_sim,0,TS,0,_24+192+y,pix);
        run(do_sim,0,XX+ADD,0,-1,NULL);
#else
        const int TS = 228, BR = 32, early_timings = 1;
        run(do_sim,0,26-early_timings,0,-1,NULL), run(do_sim,1,TS-26-early_timings,0,-1,NULL);
        for( int y = 2; y <  64; ++y )  run(do_sim,0,TS,0, y-(64-_24),pix);
        for( int y = 0; y < 192; ++y )  run(do_sim,0,BR,0, _24+y,pix),run(do_sim,0,128,BR*2, _24+y,pix),run(do_sim,0,TS-128-BR,(128+BR)*2, _24+y,pix);
        for( int y = 0; y <  56; ++y )  run(do_sim,0,TS,0, _24+192+y,pix);
#endif

        // @fixme: ADD should be 0
        // ADD(2) fixes bonanzabros.dsk. why?
        // ADD(3) crashes borderbreak. 
        // see: megashock128, bonanzabros.dsk, borderbreak128.sna
    }

    // emulate faulty ZX_HAL10H8 chip as used in 128/+2 models
    // http://www.worldofspectrum.org/forums/showthread.php?t=38284
    // https://sinclair.wiki.zxnet.co.uk/wiki/ZX_Spectrum_128#HAL_bugs
    // seen: 2 frames after loading BlackLamp(128), 3000 frames while loading HuntForRedOctober(128)
    if( do_sim )
    if( ZX_HAL10H8 )
    if( (ZX|ZX_PENTAGON) == 128 || ZX == 200 ) {
        static int seen = 0;
        if( I(cpu) & 0xC0 ) {
            int bank = (page128 & 7);
            int is_contended_bank = bank & 1; // Banks 1, 3, 5, 7 are contended
            int doit = 0;
            if( I(cpu) >= 0x40 && I(cpu) < 0x80 ) doit = 1; // @todo: ula corruption instead? [0x4000-0x7fff]
            if( I(cpu) >= 0xC0 && is_contended_bank ) doit = 1; // contended bank in [0xc000-0xffff]
            if( doit ) {
                seen = (seen + 1) * !!(I(cpu) & 0xC0);
                printf("HAL10 I=$%02x[bank:%d] (seen %d frames)\n", I(cpu), I(cpu) < 0xC0 ? 2 : bank, seen);
                // PC(cpu) = 0;
            } else {
                seen = 0;
            }
        } else {
            seen = 0;
        }
    }

    if( drawmode <= 0 )
        return;

#if 1
    // detect ZX_RF flip-flop
    static int ZX_RF_old = 1;
    int refresh = ZX_RF ^ ZX_RF_old;
    ZX_RF_old = ZX_RF;

    if(ZX_RF) {
        static window *blend = 0;
        if(!blend) blend = tigrBitmap(_256, _192);

        // reset bitmap contents when re-enabling ZX_RF
        if( refresh ) tigrBlit(blend, canvas, 0,0, 0,0, _256,_192);

        // ghosting
        tigrBlitAlpha(blend, canvas, 0,0, 0,0, _256,_192, 0.5f); //0.15f);
        tigrBlit(canvas, blend, 0,0, 0,0, _256,_192);

        // scanlines
        scanlines(canvas);

        // aberrations
        flick_hz ^= 1; // &1; // (rand()<(RAND_MAX/255));
        flick_frame = cpu.r; // &1; // (rand()<(RAND_MAX/255));
        blur(canvas);
    }
#endif
}

const char *shader_spectral = 
#if 0
"/* HSV from/to RGB conversion functions by Inigo Quilez. https://www.shadertoy.com/view/lsS3Wc (MIT licensed)*/\n"
"const float eps = 0.0000001;\n"
"vec3 hsv2rgb( vec3 c ) {\n"
"    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );\n"
"    return c.z * mix( vec3(1.0), rgb, c.y);\n"
"}\n"
"vec3 rgb2hsv( vec3 c) {\n"
"    vec4 k = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);\n"
"    vec4 p = mix(vec4(c.zy, k.wz), vec4(c.yz, k.xy), (c.z<c.y) ? 1.0 : 0.0);\n"
"    vec4 q = mix(vec4(p.xyw, c.x), vec4(c.x, p.yzx), (p.x<c.x) ? 1.0 : 0.0);\n"
"    float d = q.x - min(q.w, q.y);\n"
"    return vec3(abs(q.z + (q.w - q.y) / (6.0*d+eps)), d / (q.x+eps), q.x);\n"
"}\n"

"/* YUV conversions by icalvin102 https://www.shadertoy.com/view/3lycWz */\n"
"vec3 rgb2yuv(vec3 rgb){\n"
"    float y = 0.299*rgb.r + 0.587*rgb.g + 0.114*rgb.b;\n"
"    return vec3(y, 0.493*(rgb.b-y), 0.877*(rgb.r-y));\n"
"}\n"
"vec3 yuv2rgb(vec3 yuv){\n"
"    float y = yuv.x;\n"
"    float u = yuv.y;\n"
"    float v = yuv.z;   \n"
"    return vec3(\n"
"        y + 1.0/0.877*v,\n"
"        y - 0.39393*u - 0.58081*v,\n"
"        y + 1.0/0.493*u\n"
"    );\n"
"}\n"
#endif

#if 0
    " #define ALPHA 0.95\n"
    " vec4 tv_grid(vec4 color, vec2 coord) {\n"
    "    if (mod(floor(coord.x), 3.0) == 0.0) { // vertical bars\n"
    "        color *= vec4(ALPHA,ALPHA,ALPHA,1.00);\n"
    "    } else {\n" 
    "        float column = fract(coord.x / 6.0); // alternate horizontal bars\n"
    "        if(((column >= 0.166 && column <  0.5) && (mod(floor(coord.y+1.0),3.0) == 0.0)) ||\n"
    "           ((column >= 0.666 && column <= 1.0) && (mod(floor(coord.y),3.0) == 0.0)))\n"
    "           color *= vec4(ALPHA,ALPHA,ALPHA,1.00);\n"
    "    }\n"
    "    return color;\n"
    "}\n"
#endif

#if 1
"// [ref] https://jorenjoestar.github.io/post/pixel_art_filtering/\n"
"// [src] https://www.shadertoy.com/view/MllBWf CC1.0\n"
"// [!] textures are sampled with Bilinear filtering and Alpha Blending is enabled\n"
"vec4 texture_AA2( sampler2D tex, vec2 uv) {\n"
"    vec2 res = vec2(textureSize(tex,0));\n"
"    uv = uv*res;\n"
"    vec2 seam = floor(uv+0.5);\n"
"    uv = seam + clamp( (uv-seam)/fwidth(uv), -0.5, 0.5);\n"
"    return texture(tex, uv/res);\n"
"}\n"
#endif

    "/* based on code by lalaoopybee https://www.shadertoy.com/view/DlfSz8 */\n"
    "#define CURVATURE 8.2\n"
    "#define BLUR .01\n"
    "#define CA_AMT (1.0024) /*aberration near the corners. 1.01 is a lot.*/\n"
    "void fxShader(out vec4 color, in vec2 uv){\n"

#if 1
    "    /* curvature */\n"
    "    vec2 crtUV=uv*2.-1.;\n"
    "    vec2 offset=crtUV.yx/CURVATURE;\n"
    "    crtUV+=crtUV*offset*offset;\n"
    "    crtUV=crtUV*.5+.5;\n"

    "    /* edge blur */\n"
    "    vec2 edge=smoothstep(0., BLUR, crtUV)*(1.-smoothstep(1.-BLUR, 1., crtUV));\n"
#else
    "    vec2 crtUV = uv;\n"
    "    vec2 edge = vec2(1.,1.);\n"
#endif

    // apply aberration toward the screen corners. additionally:
    // - by using x3 texture() we get blurrier results (see: center of the screen).
    // - by using x3 texture_AA2() we get crispier results (see: center of the screen).
#if 0
    "    /* chromatic aberration (mid crisp): blur edges, focus center */\n"
    "    color = vec4(\n"
    "        texture_AA2(image, (crtUV-.5)*CA_AMT+.5).r,\n"
    "        texture_AA2(image, crtUV).g,\n"
    "        texture/*_AA2*/(image, (crtUV-.5)/CA_AMT+.5).b, 0.0\n"
    "    );\n"
#elif 0
    "    /* chromatic aberration (crispier): blur edges, focus center */\n"
    "    color = vec4(\n"
    "        texture_AA2(image, (crtUV-.5)*CA_AMT+.5).r,\n"
    "        texture_AA2(image, crtUV).g,\n"
    "        texture_AA2(image, (crtUV-.5)/CA_AMT+.5).b, 0.0\n"
    "    );\n"
#elif 0
    "    /* chromatic aberration (blurry): blur edges, focus center */\n"
    "    color = vec4(\n"
    "        texture(image, (crtUV-.5)*CA_AMT+.5).r,\n"
    "        texture(image, crtUV).g,\n"
    "        texture(image, (crtUV-.5)/CA_AMT+.5).b, 0.0\n"
    "    );\n"
#elif 0 // relatively crisp. no aberration
    "    color = texture_AA2(image, crtUV);\n"
#elif 0 // crisp. no aberration
    "    color = texture(image, crtUV);\n"
#elif 1 // variable blur. with aberration
    "    vec2 tex_size = vec2(textureSize(image, 0));\n"
    "    crtUV = mix(floor(crtUV * tex_size) + 0.5, crtUV * tex_size, 0.3+0.9*vec2(parameters2.y,parameters2.y)) / tex_size;\n"
    "    color = texture(image, crtUV);\n"

    "    float aberration = 0.15;\n" // off [0..1] max
    "    vec2 expand = (crtUV-.5)/(1.0+aberration/80.)+.5;\n"
    "    vec2 shrink = (crtUV-.5)*(1.0+aberration/80.)+.5;\n"
    "    vec2 shifts = crtUV + (aberration/320.);\n"
    "    color.g = texture(image, vec2(expand.x,expand.y)).g;\n"
    "    color.b = texture(image, vec2(shifts.x,crtUV.y)).b;\n"
#endif

    // saturation
    "   color *= parameters2.x;\n"


#if 1
    "    /* tv refresh line*/\n"
    "    float tvline = mod(parameters.x / -12.5, 1) - uv.y; // params.x @ 50hz, make it x12.5 slower so it's less distracting; neg sign for upwards dir\n"
    "    if(tvline > 0 && tvline < 0.03f) color.rgb -= tvline;\n"
#endif

    "    /* mix up */\n"
    "    color *= edge.x * edge.y;\n"

#if 0
    "    /* diodes */\n"
    "    vec2 fragCoord=uv*vec2(352.0*3.0,288.0*3.0);\n"
    "    if(mod(fragCoord.y, 2.)<1.) color.rgb*=.95;\n"
    "    else if(mod(fragCoord.x, 3.)<1.) color.rgb*=.95;\n"
    "    else color*=1.05;\n"
#elif 0
    "    color = tv_grid(color, gl_FragCoord.xy );\n"
#endif
    "}\n";

const char *shader_tigr = // tigr_default_fx_gl_fs;
    "void fxShader(out vec4 color, in vec2 uv) {\n"

    // blur
    "   vec2 tex_size = vec2(textureSize(image, 0));\n"
    "   uv = mix(floor(uv * tex_size) + 0.5, uv * tex_size, vec2(parameters2.y,parameters2.y)) / tex_size;\n"
    // fetch
    "   color = texture(image, uv);\n"
    // saturation
    "   color *= parameters2.x;\n"

"}\n";

char *shader = 0;

void crt(const char *fx) {
    extern window *app;
    if( fx )
    tigrSetPostShader(app, fx, strlen(fx));
}

int load_shaderbin(const char *data, int size) {
    if( data && size ) {
        if( strstr(data, " fxShader(") ) {
            if( writefile(".Spectral/Spectral.fx", data, size) ) {
                if( shader ) free(shader), shader = NULL;
                return !!memcpy(shader = malloc(size+1), data, size+1);
            }
        }
    }
    return 0;
}

int load_shader(const char *filename) {
    int size; char *data = readfile(filename, &size);
    if( data ) {
        int rc = load_shaderbin(data, size);
        free(data);
        return rc;
    }
    return 0;
}

