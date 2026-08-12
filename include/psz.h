#ifndef PSZ_H
#define PSZ_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// PSZ Header & Archive Structure Definitions
typedef struct {
    char magic[4];          // "PSZ1"
    uint32_t version;
    uint32_t key_len;
    uint32_t payload_len;
} psz_header_t;

// Function declarations for core archive handling
int psz_init(void);
void psz_cleanup(void);

// Decrypt and extract a .psz archive given the raw key bytes and archive data
int psz_extract_archive(const uint8_t *archive_data, size_t archive_len, 
                        const uint8_t *key, size_t key_len, 
                        const char *output_dir);

#ifdef __cplusplus
}
#endif

#endif // PSZ_H
