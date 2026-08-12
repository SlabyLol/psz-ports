#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "psz.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

#define MAX_FILES 32
#define MAX_PATH 256

typedef struct {
    char name[MAX_PATH];
    int is_dir;
} FileEntry;

static FileEntry file_list[MAX_FILES];
static int file_count = 0;
static int selected_index = 0;
static char current_path[MAX_PATH] = "sd:/";

static void scan_directory(const char *path) {
    file_count = 0;
    selected_index = 0;

    if (strcmp(path, "sd:/") != 0) {
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
    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    psz_init();
    scan_directory(current_path);

    while (1) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);

        if (pressed & WPAD_BUTTON_HOME) {
            break;
        }

        if (pressed & WPAD_BUTTON_UP) {
            if (selected_index > 0) selected_index--;
        }
        if (pressed & WPAD_BUTTON_DOWN) {
            if (selected_index < file_count - 1) selected_index++;
        }

        if (pressed & WPAD_BUTTON_A && file_count > 0) {
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
                printf("\nExtracting %s...\n", entry->name);
                psz_extract_archive(NULL, 0, NULL, 0, "sd:/psz_out");
            }
        }

        if (pressed & WPAD_BUTTON_2 && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            char full[MAX_PATH];
            snprintf(full, MAX_PATH, "%s/%s", current_path, entry->name);
            char out[MAX_PATH];
            snprintf(out, MAX_PATH, "%s/archive.psz", current_path);
            
            printf("\nCreating archive from %s...\n", entry->name);
            psz_make_archive(full, out, PSZ_FORMAT_PSZ);
            scan_directory(current_path);
        }

        printf("\x1b[2J\x1b[H");
        printf("========================================\n");
        printf("      PSZ Studio - Wii File Browser     \n");
        printf("========================================\n");
        printf("Path: %s\n\n", current_path);

        for (int i = 0; i < 15 && i < file_count; i++) {
            if (i == selected_index) {
                printf("> ");
            } else {
                printf("  ");
            }
            printf("[%s] %s\n", file_list[i].is_dir ? "DIR" : "FIL", file_list[i].name);
        }

        printf("\n----------------------------------------\n");
        printf("[A] Open/Extract  [2] Make Archive\n");
        printf("[HOME] Exit\n");

        VIDEO_WaitVSync();
    }

    psz_cleanup();
    return 0;
}
