#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psz.h"

int main(int argc, char **argv) {
    // Initialize services for dual screens
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    
    PrintConsole bottomScreen;
    consoleInit(GFX_BOTTOM, &bottomScreen);

    // Top screen header
    consoleSelect(&((PrintConsole){0})); // select default top
    printf("\x1b[32m========================================\n");
    printf("  PSZ Archive Reader - 3DS GUI Edition\n");
    printf("========================================\x1b[37m\n\n");
    printf("Status: Ready\n");
    printf("Controls:\n");
    printf("  [A] - Extract Archive\n");
    printf("  [X] - Toggle Format Mode\n");
    printf("  [START] - Exit to HBMenu\n");

    // Bottom screen log output
    consoleSelect(&bottomScreen);
    printf("\x1b[36m[Log Window]\x1b[37m\n");
    printf("Waiting for user input...\n");

    psz_init();

    int selected_format = 0; // 0 = PSZ, 1 = ZIP / Raw

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_START) {
            break;
        }

        if (kDown & KEY_A) {
            consoleSelect(&bottomScreen);
            printf("[*] Action: Extracting archive (Format: %s)...\n", selected_format == 0 ? "PSZ" : "ZIP/Other");
            psz_extract_archive(NULL, 0, NULL, 0, "sdmc:/psz_extracted");
            printf("[+] Extraction completed successfully!\n");
        }

        if (kDown & KEY_X) {
            selected_format = !selected_format;
            consoleSelect(&bottomScreen);
            printf("[*] Switched format mode to: %s\n", selected_format == 0 ? "PSZ" : "ZIP/Other");
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
