#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "psz.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define SUB_WIDTH     320
#define SUB_HEIGHT    240

#define MAX_FILES 64
#define MAX_PATH  256

typedef struct {
    char name[MAX_PATH];
    bool is_dir;
} FileEntry;

static FileEntry file_list[MAX_FILES];
static int file_count = 0;
static int selected_index = 0;
static char current_path[MAX_PATH] = "sdmc:/";

static void scan_directory(const char *path) {
    file_count = 0;
    selected_index = 0;
    
    if (strcmp(path, "sdmc:/") != 0) {
        snprintf(file_list[file_count].name, MAX_PATH, "..");
        file_list[file_count].is_dir = true;
        file_count++;
    }

    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL && file_count < MAX_FILES) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        
        snprintf(file_list[file_count].name, MAX_PATH, "%s", ent->d_name);
        
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", path, ent->d_name);
        DIR *sub = opendir(full_path);
        if (sub) {
            file_list[file_count].is_dir = true;
            closedir(sub);
        } else {
            file_list[file_count].is_dir = false;
        }
        file_count++;
    }
    closedir(dir);
}

int main(int argc, char **argv) {
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* topScreen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    u32 clrBg = C2D_Color32(30, 30, 46, 255);
    u32 clrHeader = C2D_Color32(49, 50, 68, 255);
    u32 clrText = C2D_Color32(205, 214, 244, 255);
    u32 clrHighlight = C2D_Color32(137, 180, 250, 255);
    u32 clrFolder = C2D_Color32(249, 226, 175, 255);
    u32 clrFile = C2D_Color32(166, 227, 161, 255);

    psz_init();
    scan_directory(current_path);

    C2D_TextBuf staticTextBuf = C2D_TextBufNew(512);
    C2D_TextBuf frameTextBuf = C2D_TextBufNew(4096);
    C2D_Text titleText, statusText;
    C2D_TextBufClear(staticTextBuf);
    C2D_TextParse(&titleText, staticTextBuf, "PSZ Studio - 3DS Touch Edition");
    C2D_TextOptimize(&titleText);
    C2D_TextParse(&statusText, staticTextBuf, "[Touch] Tap item to select/open  [SELECT] Exit");
    C2D_TextOptimize(&statusText);

    char action_status[128] = "Ready. Tap an item on touchscreen.";

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        touchPosition touch;
        hidTouchRead(&touch);

        if (kDown & KEY_SELECT) {
            break;
        }

        // Touch handling on bottom screen (320x240)
        if (kDown & KEY_TOUCH) {
            // Check if touch is within file list area (y: 30 to 210)
            if (touch.py >= 30 && touch.py <= 210) {
                int clicked_item = (touch.py - 30) / 20;
                int start_idx = selected_index - 6;
                if (start_idx < 0) start_idx = 0;
                
                int target_idx = start_idx + clicked_item;
                if (target_idx < file_count) {
                    selected_index = target_idx;
                    FileEntry *entry = &file_list[selected_index];
                    
                    if (entry->is_dir) {
                        if (strcmp(entry->name, "..") == 0) {
                            char *last_slash = strrchr(current_path, '/');
                            if (last_slash && last_slash != current_path) {
                                *last_slash = '\0';
                            }
                        } else {
                            strcat(current_path, "/");
                            strcat(current_path, entry->name);
                        }
                        scan_directory(current_path);
                    } else {
                        snprintf(action_status, sizeof(action_status), "Processed: %s", entry->name);
                        char full_file_path[MAX_PATH];
                        snprintf(full_file_path, MAX_PATH, "%s/%s", current_path, entry->name);
                        char out_psz[MAX_PATH];
                        snprintf(out_psz, MAX_PATH, "%s/archive.psz", current_path);
                        psz_make_archive(full_file_path, out_psz, PSZ_FORMAT_PSZ);
                    }
                }
            }
        }

        // D-Pad navigation support
        if (kDown & KEY_UP && selected_index > 0) selected_index--;
        if (kDown & KEY_DOWN && selected_index < file_count - 1) selected_index++;

        // Button action support ([A] to open/extract, [Y] to make archive)
        if (kDown & KEY_A && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            if (entry->is_dir) {
                if (strcmp(entry->name, "..") == 0) {
                    char *last_slash = strrchr(current_path, '/');
                    if (last_slash && last_slash != current_path) {
                        *last_slash = '\0';
                    }
                } else {
                    strcat(current_path, "/");
                    strcat(current_path, entry->name);
                }
                scan_directory(current_path);
            } else {
                snprintf(action_status, sizeof(action_status), "Extracted: %s", entry->name);
                char full_file_path[MAX_PATH];
                snprintf(full_file_path, MAX_PATH, "%s/%s", current_path, entry->name);
                psz_extract_archive(full_file_path, strlen(full_file_path), NULL, 0, current_path);
            }
        }

        if (kDown & KEY_Y && file_count > 0) {
            FileEntry *entry = &file_list[selected_index];
            snprintf(action_status, sizeof(action_status), "Archived: %s", entry->name);
            char full_file_path[MAX_PATH];
            snprintf(full_file_path, MAX_PATH, "%s/%s", current_path, entry->name);
            char out_psz[MAX_PATH];
            snprintf(out_psz, MAX_PATH, "%s/archive.psz", current_path);
            psz_make_archive(full_file_path, out_psz, PSZ_FORMAT_PSZ);
        }

        C2D_TextBufClear(frameTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        // Top Screen
        C2D_SceneBegin(topScreen);
        C2D_TargetClear(topScreen, clrBg);
        C2D_DrawRectSolid(0, 0, 0, SCREEN_WIDTH, 35, clrHeader);
        C2D_DrawText(&titleText, C2D_WithColor, 10, 8, 0.0f, 0.6f, 0.6f, clrText);

        C2D_DrawRectSolid(20, 50, 0, SCREEN_WIDTH - 40, 60, clrHeader);
        C2D_Text statusTextDynamic;
        C2D_TextParse(&statusTextDynamic, frameTextBuf, action_status);
        C2D_TextOptimize(&statusTextDynamic);
        C2D_DrawText(&statusTextDynamic, C2D_WithColor, 30, 65, 0.0f, 0.5f, 0.5f, clrHighlight);
        C2D_DrawText(&statusText, C2D_WithColor, 20, 200, 0.0f, 0.45f, 0.45f, clrText);

        // Bottom Screen (File Browser)
        C2D_SceneBegin(bottomScreen);
        C2D_TargetClear(bottomScreen, clrBg);

        C2D_DrawRectSolid(0, 0, 0, SUB_WIDTH, 25, clrHeader);
        C2D_Text pathText;
        C2D_TextParse(&pathText, frameTextBuf, current_path);
        C2D_TextOptimize(&pathText);
        C2D_DrawText(&pathText, C2D_WithColor, 5, 4, 0.0f, 0.45f, 0.45f, clrHighlight);

        int start_idx = selected_index - 6;
        if (start_idx < 0) start_idx = 0;
        int max_visible = 9;

        for (int i = 0; i < max_visible && (start_idx + i) < file_count; i++) {
            int idx = start_idx + i;
            FileEntry *entry = &file_list[idx];
            float y = 30 + (i * 20);

            if (idx == selected_index) {
                C2D_DrawRectSolid(0, y, 0, SUB_WIDTH, 18, clrHeader);
            }

            C2D_Text itemText;
            C2D_TextParse(&itemText, frameTextBuf, entry->name);
            C2D_TextOptimize(&itemText);
            
            u32 col = entry->is_dir ? clrFolder : clrFile;
            C2D_DrawText(&itemText, C2D_WithColor, 10, y + 2, 0.0f, 0.5f, 0.5f, col);
        }

        C3D_FrameEnd(0);
    }

    C2D_TextBufDelete(frameTextBuf);
    C2D_TextBufDelete(staticTextBuf);
    psz_cleanup();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
