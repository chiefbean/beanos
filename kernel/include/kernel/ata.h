#ifndef _KERNEL_ATA_H
#define _KERNEL_ATA_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t present;
    uint32_t sectors;
    char model[41];
} ata_drive_t;

uintptr_t ata_identify();
char* ata_read_sector(uint32_t lba, ata_drive_t* drive, char* buf);
void* ata_write_sector(uint32_t lba, ata_drive_t* drive, char* buf);

#ifdef __cplusplus
}
#endif

#endif