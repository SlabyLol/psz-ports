#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psz.h"

int main(int argc, char **argv) {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("\x1b[32m========================================\n");
    printf("  PSZ Archive Reader for Nintendo 3DS\n");
    printf("========================================\x1b[37m\n\n");

    psz_init();
    printf("[*] PSZ Core initialized successfully.\n");
    printf("[*] Press START to exit.\n\n");

    // Main loop
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_START) {
            break; // break in order to return to hbmenu
        }

        // Flush and swap frames
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    psz_cleanup();
    gfxExit();
    return 0;
}
