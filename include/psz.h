#ifndef PSZ_H
#define PSZ_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Supported Archive / File Formats
typedef enum {
    PSZ_FORMAT_PSZ = 0,     // PSZ Archive (.psz)
    PSZ_FORMAT_ZIP = 1,     // Standard ZIP Archive
    PSZ_FORMAT_TAR = 2,     // Uncompressed TAR Archive
    PSZ_FORMAT_CUSTOM = 3   // Custom Metadata / Binary Format
} psz_format_t;

// Exact Python psz layout: MAGIC (4) + VERSION (1) + NONCE (12) + DATA
#define PSZ_MAGIC "PSZ1"
#define PSZ_VERSION 1
#define PSZ_NONCE_SIZE 12

typedef struct {
    char magic[4];          // "PSZ1"
    uint8_t version;        // 1
    uint8_t nonce[12];      // 12-byte nonce / IV placeholder
} psz_file_header_t;

// Function declarations for core archive handling
int psz_init(void);
void psz_cleanup(void);

// Detect format based on magic bytes
psz_format_t psz_detect_format(const uint8_t *data, size_t len);

// Extraction (reads .psz, unpacks payload)
int psz_extract_archive(const uint8_t *archive_data, size_t archive_len, 
                        const uint8_t *key, size_t key_len, 
                        const char *output_dir);

// Creation ("make archive" - packs file/folder into .psz format)
int psz_make_archive(const char *source_path, const char *output_psz_path, psz_format_t format);

#ifdef __cplusplus
}
#endif

#endif // PSZ_H
