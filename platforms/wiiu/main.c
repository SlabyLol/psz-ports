#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <whb/log.h>
#include <whb/proc.h>
#include <coreinit/screen.h>
#include "psz.h"

int main(int argc, char **argv) {
    WHBProcInit();
    WHBLogPrint("PSZ Archive Reader for Nintendo Wii U initialized.");

    psz_init();

    // Main loop
    while (WHBProcIsRunning()) {
        // Render or handle events
        WHBProcWaitForEvent();
    }

    psz_cleanup();
    WHBProcShutdown();
    return 0;
}
