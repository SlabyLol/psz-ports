#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/psz.h"

void test_format_detection() {
    printf("[TEST] Running format detection tests...\n");
    
    uint8_t psz_magic[8] = {'P', 'S', 'Z', '1', 0x00, 0x01, 0x02, 0x03};
    assert(psz_detect_format(psz_magic, 8) == PSZ_FORMAT_PSZ);

    uint8_t zip_magic[8] = {'P', 'K', 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};
    assert(psz_detect_format(zip_magic, 8) == PSZ_FORMAT_ZIP);

    printf("[TEST] Format detection passed successfully!\n");
}

void test_archive_creation_and_extraction() {
    printf("[TEST] Running archive creation and extraction tests...\n");

    const char *test_src = "/tmp/test_input.txt";
    const char *test_archive = "/tmp/test_output.psz";
    const char *extract_dir = "/tmp";

    // Create dummy source file
    FILE *f = fopen(test_src, "w");
    fprintf(f, "Hello PSZ Homebrew World!");
    fclose(f);

    // Make archive
    int make_res = psz_make_archive(test_src, test_archive, PSZ_FORMAT_PSZ);
    assert(make_res == 0);

    // Read archive back
    FILE *arch_file = fopen(test_archive, "rb");
    assert(arch_file != NULL);
    fseek(arch_file, 0, SEEK_END);
    long arch_size = ftell(arch_file);
    fseek(arch_file, 0, SEEK_SET);

    uint8_t *arch_buf = malloc(arch_size);
    assert(fread(arch_buf, 1, (size_t)arch_size, arch_file) == (size_t)arch_size);
    fclose(arch_file);

    // Extract archive
    int ext_res = psz_extract_archive(arch_buf, arch_size, NULL, 0, extract_dir);
    assert(ext_res == 0);

    free(arch_buf);
    printf("[TEST] Archive creation & extraction passed successfully!\n");
}

int main() {
    printf("========================================\n");
    printf("     PSZ Core Automated Unit Tests      \n");
    printf("========================================\n");

    psz_init();
    test_format_detection();
    test_archive_creation_and_extraction();
    psz_cleanup();

    printf("\n[SUCCESS] All unit tests passed without errors!\n");
    return 0;
}
