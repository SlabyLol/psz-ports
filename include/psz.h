#ifndef PSZ_H
#define PSZ_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Supported Archive / File Formats
typedef enum {
    PSZ_FORMAT_PSZ = 0,     // Encrypted PSZ Archive (AES-256-GCM)
    PSZ_FORMAT_ZIP = 1,     // Standard ZIP Archive
    PSZ_FORMAT_TAR = 2,     // Uncompressed TAR Archive
    PSZ_FORMAT_CUSTOM = 3   // Custom Metadata / Binary Format
} psz_format_t;

// PSZ Header & Archive Structure Definitions
typedef struct {
    char magic[4];          // "PSZ1"
    uint32_t version;
    uint32_t key_len;
    uint32_t payload_len;
    psz_format_t format_type;
} psz_header_t;

// Function declarations for core archive handling
int psz_init(void);
void psz_cleanup(void);

// Detect format based on magic bytes
psz_format_t psz_detect_format(const uint8_t *data, size_t len);

// Extraction
int psz_extract_archive(const uint8_t *archive_data, size_t archive_len, 
                        const uint8_t *key, size_t key_len, 
                        const char *output_dir);

// Creation ("make archive")
int psz_make_archive(const char *source_path, const char *output_psz_path, psz_format_t format);

#ifdef __cplusplus
}
#endif

#endif // PSZ_H
