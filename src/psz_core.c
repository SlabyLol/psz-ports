#include "psz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    } else if (memcmp(data, "ustar", 5) == 2) {
        return PSZ_FORMAT_TAR;
    }

    return PSZ_FORMAT_CUSTOM;
}

int psz_extract_archive(const uint8_t *archive_data, size_t archive_len, 
                        const uint8_t *key, size_t key_len, 
                        const char *output_dir) {
    if (!archive_data || archive_len < sizeof(psz_header_t)) {
        return -1;
    }

    psz_header_t *header = (psz_header_t *)archive_data;
    if (memcmp(header->magic, "PSZ1", 4) != 0) {
        return -1;
    }

    // Write out payload to output file / directory
    char out_filepath[512];
    snprintf(out_filepath, sizeof(out_filepath), "%s/extracted_payload.bin", output_dir);
    
    FILE *f = fopen(out_filepath, "wb");
    if (!f) return -1;

    const uint8_t *payload_ptr = archive_data + sizeof(psz_header_t);
    size_t payload_size = archive_len - sizeof(psz_header_t);
    
    fwrite(payload_ptr, 1, payload_size, f);
    fclose(f);

    return 0;
}

int psz_make_archive(const char *source_path, const char *output_psz_path, psz_format_t format) {
    // Open source file to read its content
    FILE *src = fopen(source_path, "rb");
    if (!src) {
        return -1;
    }

    fseek(src, 0, SEEK_END);
    long src_size = ftell(src);
    fseek(src, 0, SEEK_SET);

    uint8_t *src_buf = malloc(src_size);
    if (!src_buf) {
        fclose(src);
        return -1;
    }

    if (fread(src_buf, 1, (size_t)src_size, src) != (size_t)src_size) {
        fclose(src);
        free(src_buf);
        return -1;
    }
    fclose(src);

    // Create output .psz file
    FILE *out = fopen(output_psz_path, "wb");
    if (!out) {
        free(src_buf);
        return -1;
    }

    psz_header_t header;
    memcpy(header.magic, "PSZ1", 4);
    header.version = 1;
    header.key_len = 32;
    header.payload_len = (uint32_t)src_size;
    header.format_type = format;

    // Write header and payload
    fwrite(&header, sizeof(psz_header_t), 1, out);
    fwrite(src_buf, 1, src_size, out);

    fclose(out);
    free(src_buf);

    return 0;
}
