#ifndef _KERNEL_FAT16_H
#define _KERNEL_FAT16_H 1

#include <kernel/vfs.h>
#include <kernel/ata.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t jump[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t media;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint8_t drive_number;
    uint8_t reserved1;
    uint8_t boot_signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t fs_type[8];
} __attribute__((packed)) bpb_t;

typedef struct {
    ata_drive_t* drive;
    uint16_t fat_start;
    uint16_t root_start;
    uint16_t root_sectors;
    uint16_t data_start;
    uint16_t bytes_per_sector;
    uint16_t sectors_per_cluster;
    uint8_t num_fats;
    uint16_t sectors_per_fat;
} fat16_fs_t;

typedef struct {
    uint32_t dir_lba;
    uint16_t dir_off;
} fat16_node_t;

typedef struct {
    uint8_t name[11];
    uint8_t attr;
    uint8_t reserved[14];
    uint16_t inode;
    uint32_t file_size;
} __attribute__((packed)) fat16_dirent_t;

vfs_node_t* fat16_mount(bpb_t* bpb, ata_drive_t* drive);
char* fat16_read(vfs_node_t* node, size_t offset, size_t size, char* buf);
void* fat16_write(vfs_node_t* node, size_t offset, size_t size, char* buf);
vfs_node_t* fat16_open(vfs_node_t* node);
void* fat16_close(vfs_node_t* node);
dirent_t* fat16_readdir(vfs_node_t* node, size_t index);
vfs_node_t* fat16_finddir(vfs_node_t* node, char* name);
vfs_node_t* fat16_create(vfs_node_t* dir, char* name, uint32_t flags);
void* fat16_delete(vfs_node_t* node);

#ifdef __cplusplus
}
#endif

#endif