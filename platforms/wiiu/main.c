#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <whb/log.h>
#include <whb/proc.h>
#include <coreinit/screen.h>
#include <padscore/kpad.h>
#include "psz.h"

int main(int argc, char **argv) {
    WHBProcInit();
    KPADInit();

    // Initialize screen buffers for TV and GamePad
    OSScreenInit();
    u32 tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    void *tvBuffer = memalign(0x100, tvBufferSize);
    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenEnableEx(SCREEN_TV, true);

    WHBLogPrint("PSZ Archive Reader - Wii U GUI Edition initialized.");
    printf("[Wii U] PSZ Archive Reader started. Press Home on GamePad to exit.\n");

    psz_init();

    int mode = 0;

    while (WHBProcIsRunning()) {
        KPADData padData;
        KPADRead(WPAD_CHAN_0, &padData, 1);

        if (padData.trigger & KPAD_BUTTON_A) {
            WHBLogPrint("[*] Wii U: Extracting archive...");
            psz_extract_archive(NULL, 0, NULL, 0, "/vol/storage_mlc01/psz_out");
        }

        if (padData.trigger & KPAD_BUTTON_X) {
            mode = !mode;
            WHBLogPrint("[*] Wii U: Format mode toggled.");
        }

        WHBProcWaitForEvent();
    }

    psz_cleanup();
    free(tvBuffer);
    OSScreenShutdown();
    KPADShutdown();
    WHBProcShutdown();
    return 0;
}
