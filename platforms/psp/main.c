#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "psz.h"

PSP_MODULE_INFO("PSZ Studio PSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define MAX_FILES 32
#define MAX_PATH 256

typedef struct {
    char name[MAX_PATH];
    int is_dir;
} FileEntry;

static FileEntry file_list[MAX_FILES];
static int file_count = 0;
static int selected_index = 0;
static char current_path[MAX_PATH] = "ms0:/";

int exit_callback(int arg1, int arg2, void *common) {
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

static void scan_directory(const char *path) {
    file_count = 0;
    selected_index = 0;

    if (strcmp(path, "ms0:/") != 0) {
        snprintf(file_list[file_count].name, MAX_PATH, "..");
        file_list[file_count].is_dir = 1;
        file_count++;
    }

    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL && file_count < MAX_FILES) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        snprintf(file_list[file_count].name, MAX_PATH, "%s", ent->d_name);
        
        char full[MAX_PATH];
        snprintf(full, MAX_PATH, "%s/%s", path, ent->d_name);
        DIR *sub = opendir(full);
        if (sub) {
            file_list[file_count].is_dir = 1;
            closedir(sub);
        } else {
            file_list[file_count].is_dir = 0;
        }
        file_count++;
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    pspDebugScreenInit();
    SetupCallbacks();

    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    psz_init();
    scan_directory(current_path);

    SceCtrlData pad;
    int last_buttons = 0;

    while (1) {
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf("========================================\n");
        pspDebugScreenPrintf("     PSZ Studio - PSP File Browser      \n");
        pspDebugScreenPrintf("========================================\n");
        pspDebugScreenPrintf("Path: %s\n\n", current_path);

        for (int i = 0; i < 15 && i < file_count; i++) {
            if (i == selected_index) {
                pspDebugScreenPrintf("> ");
            } else {
                pspDebugScreenPrintf("  ");
            }
            pspDebugScreenPrintf("[%s] %s\n", file_list[i].is_dir ? "DIR" : "FIL", file_list[i].name);
        }

        pspDebugScreenPrintf("\n----------------------------------------\n");
        pspDebugScreenPrintf("[X] Open/Extract  [TRIANGLE] Make Archive\n");
        pspDebugScreenPrintf("[START] Exit\n");

        sceCtrlReadBufferPositive(&pad, 1);
        int pressed = pad.Buttons & ~last_buttons;
        last_buttons = pad.Buttons;

        if (pad.Buttons & PSP_CTRL_START) {
            break;
        }

        if (pressed & PSP_CTRL_UP) {
            if (selected_index > 0) selected_index--;
        }
        if (pressed & PSP_CTRL_DOWN) {
            if (selected_index < file_count - 1) selected_index++;
        }

        if (pressed & PSP_CTRL_CROSS && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            if (entry->is_dir) {
                if (strcmp(entry->name, "..") == 0) {
                    char *slash = strrchr(current_path, '/');
                    if (slash && slash != current_path) *slash = '\0';
                } else {
                    strcat(current_path, "/");
                    strcat(current_path, entry->name);
                }
                scan_directory(current_path);
            } else {
                char full[MAX_PATH];
                snprintf(full, MAX_PATH, "%s/%s", current_path, entry->name);
                pspDebugScreenPrintf("\nExtracting %s...\n", entry->name);
                psz_extract_archive(NULL, 0, NULL, 0, "ms0:/psz_out");
                sceKernelDelayThread(1500000);
            }
        }

        if (pressed & PSP_CTRL_TRIANGLE && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            char full[MAX_PATH];
            snprintf(full, MAX_PATH, "%s/%s", current_path, entry->name);
            char out[MAX_PATH];
            snprintf(out, MAX_PATH, "%s/archive.psz", current_path);
            
            pspDebugScreenPrintf("\nCreating archive from %s...\n", entry->name);
            psz_make_archive(full, out, PSZ_FORMAT_PSZ);
            sceKernelDelayThread(1500000);
            scan_directory(current_path);
        }

        sceKernelDelayThread(50000);
    }

    psz_cleanup();
    sceKernelExitGame();
    return 0;
}
