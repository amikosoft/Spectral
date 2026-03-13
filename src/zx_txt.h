/*
"ABORT","ANNULLA","BOMB","BOMBA","CONTROLS","CONTROLLI","CURSOR","CURSORI","DEFINE","DEFINISCI","DEFINE","RIDEFINISCI","DOWN","GIU","FIRE","SPARO","FIRE","FUOCO","GAME","GIOCA","INSTRUCTIONS","ISTRUZIONI","JUMP","SALTA","JUMP","SALTO","KEYBOARD","TASTIERA","KEYPAD","TASTIERA","KEYS","TASTI","LEFT","SINISTRA","LIFES","VITE","PAUSE","PAUSA","PICK","RACCOGLI","PLAY","GIOCA","PRESS
ANY KEY","PREMI UN
TASO","RESET","RIPARTI","RIGHT","DESTRA","START","INIZIA","UP","SU","USE","USA",
*/

    // es2en
    const char* tx[][2] = { // important: list must be sorted
        {"ABAJO","DOWN"},
        {"ABORTA","ABORT"},
        {"ABORTAR","ABORT"},
        {"ARRIBA","UP"},
        {"BOMBA","BOMB"},
        {"COGER","PICK"},
        {"COMENZAR","START"},
        {"CONTROLES","CONTROLS"},
        {"CURSORES","CURSOR"},
        {"DEFINIR","DEFINE"},
        {"DERECHA","RIGHT"},
        {"DISPARO","FIRE"},
        {"EMPEZAR","START"},
        {"FUEGO","FIRE"},
        {"INSTRUCCIONES","INSTRUCTIONS"},
        {"IZQUIERDA", "LEFT"},
        {"JUEGO","GAME"},
        {"JUGAR","PLAY"},
        {"PAUSA","PAUSE"},
        {"PULSA UNA TECLA", "PRESS ANY KEY"},
        {"REDEFINIR", "DEFINE"},
        {"REINICIA","RESET"},
        {"REINICIAR", "RESET"},
        {"SALTAR","JUMP"},
        {"SALTO","JUMP"},
        {"TECLADO","KEYPAD"}, // cant use 'keyboard', as it is longer than 'teclado'
        {"TECLAS","KEYS"},
        //{"USAR", "USE"},
        {"VIDAS","LIFES"},
    };
