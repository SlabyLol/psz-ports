#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <malloc.h>
#include <whb/log.h>
#include <whb/proc.h>
#include <coreinit/screen.h>
#include <padscore/kpad.h>
#include "psz.h"

#define MAX_FILES 32
#define MAX_PATH 256

typedef struct {
    char name[MAX_PATH];
    int is_dir;
} FileEntry;

static FileEntry file_list[MAX_FILES];
static int file_count = 0;
static int selected_index = 0;
static char current_path[MAX_PATH] = "fs:/vol/external01";

static void scan_directory(const char *path) {
    file_count = 0;
    selected_index = 0;

    if (strcmp(path, "fs:/vol/external01") != 0) {
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

int main(int argc, char **argv) {
    WHBProcInit();
    KPADInit();
    OSScreenInit();

    u32 tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    void *tvBuffer = memalign(0x100, tvBufferSize);
    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenEnableEx(SCREEN_TV, true);

    u32 drcBufferSize = OSScreenGetBufferSizeEx(SCREEN_DRC);
    void *drcBuffer = memalign(0x100, drcBufferSize);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);
    OSScreenEnableEx(SCREEN_DRC, true);

    psz_init();
    scan_directory(current_path);

    WHBLogPrint("PSZ Studio - Wii U started.");

    while (WHBProcIsRunning()) {
        KPADData padData;
        KPADRead(WPAD_CHAN_0, &padData, 1);

        u32 pressed = padData.trigger;

        if (pressed & KPAD_BUTTON_UP) {
            if (selected_index > 0) selected_index--;
        }
        if (pressed & KPAD_BUTTON_DOWN) {
            if (selected_index < file_count - 1) selected_index++;
        }

        if (pressed & KPAD_BUTTON_A && file_count > 0) {
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
                WHBLogPrint("[*] Extracting archive...");
                psz_extract_archive(NULL, 0, NULL, 0, "fs:/vol/external01/psz_out");
            }
        }

        if (pressed & KPAD_BUTTON_X && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            char full[MAX_PATH];
            snprintf(full, MAX_PATH, "%s/%s", current_path, entry->name);
            char out[MAX_PATH];
            snprintf(out, MAX_PATH, "%s/archive.psz", current_path);
            
            WHBLogPrint("[*] Creating archive...");
            psz_make_archive(full, out, PSZ_FORMAT_PSZ);
            scan_directory(current_path);
        }

        // Render on TV and DRC
        for (int screen = 0; screen < 2; screen++) {
            OSScreenClearBufferEx(screen, 0x1E1E2EFF);
            
            // Print Header
            // OSScreenPutFontEx(screen, 2, 2, "========================================");
            // OSScreenPutFontEx(screen, 2, 3, "      PSZ Studio - Wii U Browser        ");
            // OSScreenPutFontEx(screen, 2, 4, "========================================");

            char path_buf[280];
            snprintf(path_buf, sizeof(path_buf), "Path: %s", current_path);
            OSScreenPutFontEx(screen, 2, 6, path_buf);

            for (int i = 0; i < 15 && i < file_count; i++) {
                char line[300];
                snprintf(line, sizeof(line), "%s [%s] %s", 
                         (i == selected_index) ? ">" : " ", 
                         file_list[i].is_dir ? "DIR" : "FIL", 
                         file_list[i].name);
                OSScreenPutFontEx(screen, 2, 8 + i, line);
            }

            OSScreenPutFontEx(screen, 2, 24, "[A] Open/Extract  [X] Make Archive  [HOME] Exit");

            OSScreenFlipBuffersEx(screen);
        }

        WHBProcWaitForEvent();
    }

    psz_cleanup();
    free(tvBuffer);
    free(drcBuffer);
    OSScreenShutdown();
    KPADShutdown();
    WHBProcShutdown();
    return 0;
}
