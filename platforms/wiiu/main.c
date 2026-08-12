#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <malloc.h>
#include <coreinit/screen.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <vpad/input.h>
#include <whb/proc.h>
#include "psz.h"

#define MAX_FILES 32
#define MAX_PATH 256
#define VISIBLE_FILES 15

typedef struct {
    char name[MAX_PATH];
    int is_dir;
} FileEntry;

static FileEntry file_list[MAX_FILES];
static int file_count = 0;
static int selected_index = 0;
static char current_path[MAX_PATH] = "fs:/vol/external01";
static char status_line[128] = "Ready";

static void scan_directory(const char *path) {
    file_count = 0;
    selected_index = 0;

    if (strcmp(path, "fs:/vol/external01") != 0 && file_count < MAX_FILES) {
        snprintf(file_list[file_count].name, MAX_PATH, "..");
        file_list[file_count++].is_dir = 1;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        snprintf(status_line, sizeof(status_line), "Cannot open: %s", path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && file_count < MAX_FILES) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        FileEntry *item = &file_list[file_count];
        snprintf(item->name, MAX_PATH, "%s", entry->d_name);

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        DIR *subdir = opendir(full_path);
        item->is_dir = subdir != NULL;
        if (subdir) closedir(subdir);
        file_count++;
    }
    closedir(dir);
}

static void open_selected(void) {
    if (file_count == 0) return;

    FileEntry *entry = &file_list[selected_index];
    if (entry->is_dir) {
        if (!strcmp(entry->name, "..")) {
            char *slash = strrchr(current_path, '/');
            if (slash && slash != current_path) *slash = '\0';
        } else if (strlen(current_path) + strlen(entry->name) + 2 < MAX_PATH) {
            strcat(current_path, "/");
            strcat(current_path, entry->name);
        }
        scan_directory(current_path);
        return;
    }

    snprintf(status_line, sizeof(status_line), "Extracting %s", entry->name);
    char source_path[MAX_PATH];
    snprintf(source_path, sizeof(source_path), "%s/%s", current_path, entry->name);
    FILE *input = fopen(source_path, "rb");
    if (!input) {
        snprintf(status_line, sizeof(status_line), "Failed to open %s", entry->name);
        return;
    }
    fseek(input, 0, SEEK_END);
    long size = ftell(input);
    fseek(input, 0, SEEK_SET);
    if (size <= 0) {
        fclose(input);
        snprintf(status_line, sizeof(status_line), "Empty input file");
        return;
    }

    uint8_t *buffer = malloc((size_t) size);
    if (!buffer || fread(buffer, 1, (size_t) size, input) != (size_t) size) {
        fclose(input);
        free(buffer);
        snprintf(status_line, sizeof(status_line), "Read failed");
        return;
    }
    fclose(input);

    int result = psz_extract_archive(buffer, (size_t) size, NULL, 0, "fs:/vol/external01/psz_out");
    free(buffer);
    snprintf(status_line, sizeof(status_line), result == 0 ? "Extraction complete" : "Unsupported or invalid archive");
}

static void create_archive(void) {
    if (file_count == 0) return;
    FileEntry *entry = &file_list[selected_index];
    if (entry->is_dir) {
        snprintf(status_line, sizeof(status_line), "Select a file to archive");
        return;
    }

    char source_path[MAX_PATH];
    char output_path[MAX_PATH];
    snprintf(source_path, sizeof(source_path), "%s/%s", current_path, entry->name);
    snprintf(output_path, sizeof(output_path), "%s/archive.psz", current_path);
    int result = psz_make_archive(source_path, output_path, PSZ_FORMAT_PSZ);
    snprintf(status_line, sizeof(status_line), result == 0 ? "Created archive.psz" : "Archive creation failed");
    if (result == 0) scan_directory(current_path);
}

static void draw_screen(OSScreenID screen) {
    OSScreenClearBufferEx(screen, 0x1E1E2EFF);
    OSScreenPutFontEx(screen, 2, 2, "PSZ Studio - Wii U");
    OSScreenPutFontEx(screen, 2, 4, current_path);

    for (int i = 0; i < VISIBLE_FILES && i < file_count; i++) {
        char row[300];
        snprintf(row, sizeof(row), "%s [%s] %s", i == selected_index ? ">" : " ",
                 file_list[i].is_dir ? "DIR" : "FILE", file_list[i].name);
        OSScreenPutFontEx(screen, 2, 6 + i, row);
    }

    OSScreenPutFontEx(screen, 2, 23, status_line);
    OSScreenPutFontEx(screen, 2, 25, "A / tap: open   X: make archive   HOME: exit");
    OSScreenFlipBuffersEx(screen);
}

int main(int argc, char **argv) {
    WHBProcInit();
    OSScreenInit();

    uint32_t tv_size = OSScreenGetBufferSizeEx(SCREEN_TV);
    uint32_t drc_size = OSScreenGetBufferSizeEx(SCREEN_DRC);
    void *tv_buffer = memalign(0x100, tv_size);
    void *drc_buffer = memalign(0x100, drc_size);
    if (!tv_buffer || !drc_buffer) return 1;
    OSScreenSetBufferEx(SCREEN_TV, tv_buffer);
    OSScreenSetBufferEx(SCREEN_DRC, drc_buffer);
    OSScreenEnableEx(SCREEN_TV, true);
    OSScreenEnableEx(SCREEN_DRC, true);

    psz_init();
    scan_directory(current_path);
    bool touch_was_down = false;

    while (WHBProcIsRunning()) {
        VPADStatus pad;
        VPADReadError error;
        int32_t samples = VPADRead(VPAD_CHAN_0, &pad, 1, &error);
        if (samples > 0) {
            if (pad.trigger & VPAD_BUTTON_UP && selected_index > 0) selected_index--;
            if (pad.trigger & VPAD_BUTTON_DOWN && selected_index + 1 < file_count) selected_index++;
            if (pad.trigger & VPAD_BUTTON_A) open_selected();
            if (pad.trigger & VPAD_BUTTON_X) create_archive();

            bool touch_now_down = pad.tpNormal.touched != 0;
            if (touch_now_down && !touch_was_down) {
                VPADTouchData calibrated;
                VPADGetTPCalibratedPoint(VPAD_CHAN_0, &calibrated, &pad.tpNormal);
                int touch_row = (int)(calibrated.y / 18);
                if (touch_row >= 0 && touch_row < file_count && touch_row < VISIBLE_FILES) {
                    selected_index = touch_row;
                    open_selected();
                }
            }
            touch_was_down = touch_now_down;
        }

        draw_screen(SCREEN_TV);
        draw_screen(SCREEN_DRC);
        OSSleepTicks(OSMillisecondsToTicks(16));
    }

    psz_cleanup();
    free(tv_buffer);
    free(drc_buffer);
    OSScreenShutdown();
    WHBProcShutdown();
    return 0;
}
