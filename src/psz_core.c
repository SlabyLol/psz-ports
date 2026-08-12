#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psz.h"

int psz_init(void) {
    return 0;
}

void psz_cleanup(void) {
    // Cleanup
}

psz_format_t psz_detect_format(const uint8_t *data, size_t len) {
    if (!data || len < 4) {
        return PSZ_FORMAT_PSZ;
    }

    if (memcmp(data, "PSZ1", 4) == 0) {
        return PSZ_FORMAT_PSZ;
    } else if (memcmp(data, "PK\x03\x04", 4) == 0) {
        return PSZ_FORMAT_ZIP;
    } else if (len > 257 && memcmp(data + 257, "ustar", 5) == 0) {
        return PSZ_FORMAT_TAR;
    }

    return PSZ_FORMAT_CUSTOM;
}

int psz_extract_archive(const uint8_t *archive_data, size_t archive_len, 
                        const uint8_t *key, size_t key_len, 
                        const char *output_dir) {
    if (!archive_data || archive_len < (4 + 1 + PSZ_NONCE_SIZE)) {
        return -1;
    }

    if (memcmp(archive_data, PSZ_MAGIC, 4) != 0) {
        return -1;
    }

    uint8_t version = archive_data[4];
    if (version != PSZ_VERSION) {
        return -1;
    }

    const uint8_t *payload_ptr = archive_data + 4 + 1 + PSZ_NONCE_SIZE;
    size_t payload_len = archive_len - (4 + 1 + PSZ_NONCE_SIZE);

    // Write extracted payload
    char out_filepath[512];
    snprintf(out_filepath, sizeof(out_filepath), "%s/extracted_payload.bin", output_dir);
    
    FILE *f = fopen(out_filepath, "wb");
    if (!f) return -1;
    
    fwrite(payload_ptr, 1, payload_len, f);
    fclose(f);

    return 0;
}

int psz_make_archive(const char *source_path, const char *output_psz_path, psz_format_t format) {
    FILE *src = fopen(source_path, "rb");
    if (!src) {
        return -1;
    }

    fseek(src, 0, SEEK_END);
    long src_size = ftell(src);
    fseek(src, 0, SEEK_SET);

    uint8_t *src_buf = malloc(src_size > 0 ? src_size : 1);
    if (!src_buf) {
        fclose(src);
        return -1;
    }

    if (src_size > 0 && fread(src_buf, 1, (size_t)src_size, src) != (size_t)src_size) {
        fclose(src);
        free(src_buf);
        return -1;
    }
    fclose(src);

    FILE *out = fopen(output_psz_path, "wb");
    if (!out) {
        free(src_buf);
        return -1;
    }

    // Write exact Python psz header: MAGIC (4) + VERSION (1) + NONCE (12)
    fwrite(PSZ_MAGIC, 1, 4, out);
    uint8_t version = PSZ_VERSION;
    fwrite(&version, 1, 1, out);

    uint8_t nonce[PSZ_NONCE_SIZE];
    memset(nonce, 0x42, PSZ_NONCE_SIZE); // Deterministic/pseudo nonce for homebrew packaging
    fwrite(nonce, 1, PSZ_NONCE_SIZE, out);

    // Write payload data
    if (src_size > 0) {
        fwrite(src_buf, 1, (size_t)src_size, out);
    }

    fclose(out);
    free(src_buf);

    return 0;
}
