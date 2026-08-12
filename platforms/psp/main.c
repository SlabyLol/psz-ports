#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psz.h"

PSP_MODULE_INFO("PSZ Archive Tool", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Exit callback */
int exit_callback(int arg1, int arg2, int common) {
    sceKernelExitGame();
    return 0;
}

int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int SetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

int main(int argc, char *argv[]) {
    pspDebugScreenInit();
    SetupCallbacks();

    pspDebugScreenPrintf("========================================\n");
    pspDebugScreenPrintf("  PSZ Archive Reader for PlayStation PSP\n");
    pspDebugScreenPrintf("========================================\n\n");
    
    psz_init();
    pspDebugScreenPrintf("[*] PSZ Core initialized successfully.\n");
    pspDebugScreenPrintf("[*] Ready to decrypt and unpack .psz archives.\n\n");
    pspDebugScreenPrintf("Press X to exit.\n");

    SceCtrlData pad;
    while (1) {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_CROSS) {
            break;
        }
        sceKernelDelayThread(10000);
    }

    psz_cleanup();
    sceKernelExitGame();
    return 0;
}
