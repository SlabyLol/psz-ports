#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "psz.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
    // Initialise the video system
    VIDEO_Init();

    // Initialise the Wii controllers
    WPAD_Init();

    // Obtain the preferred video mode from the system
    rmode = VIDEO_GetPreferredMode(NULL);

    // Allocate memory for the display in the TV system
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Initialise the video framer
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_INTERLACE) {
        VIDEO_WaitVSync();
    }

    // Initialise the console
    CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    printf("\x1b[2J\x1b[H");
    printf("========================================\n");
    printf("  PSZ Archive Reader for Nintendo Wii\n");
    printf("========================================\n\n");

    psz_init();
    printf("[*] PSZ Core initialized successfully.\n");
    printf("[*] Press HOME on Wii Remote to exit.\n\n");

    while (1) {
        // Call WPAD_ScanPads each loop, this reads the remote
        WPAD_ScanPads();

        u32 pressed = WPAD_ButtonsDown(0);
        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        // Wait for next frame
        VIDEO_WaitVSync();
    }

    psz_cleanup();
    return 0;
}
