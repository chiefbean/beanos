#include <kernel/fat16.h>
#include <kernel/ata.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static fat16_fs_t fs;

vfs_node_t* fat16_mount(bpb_t* drive_info, ata_drive_t* drive) {
    fs.drive = drive;
    fs.fat_start = drive_info->reserved_sectors;
    fs.root_start =
        drive_info->reserved_sectors + drive_info->num_fats * drive_info->sectors_per_fat;
    fs.root_sectors = (drive_info->root_entry_count * 32) / drive_info->bytes_per_sector;
    fs.data_start = fs.root_start + fs.root_sectors;
    fs.bytes_per_sector = drive_info->bytes_per_sector;
    fs.sectors_per_cluster = drive_info->sectors_per_cluster;
    fs.num_fats = drive_info->num_fats;
    fs.sectors_per_fat = drive_info->sectors_per_fat;

    vfs_node_t* node = malloc(sizeof(vfs_node_t));
    if(!node) {
        return NULL;
    }
    strcpy(node->name, "/");
    node->flags = VFS_DIRECTORY;
    node->size = fs.root_sectors;
    node->inode = 0;
    node->read = fat16_read;
    node->write = fat16_write;
    node->open = fat16_open;
    node->close = fat16_close;
    node->readdir = fat16_readdir;
    node->finddir = fat16_finddir;
    node->create = fat16_create;
    node->delete = fat16_delete;

    return node;
}

uint16_t fat_lookup(uint32_t n) {
    uint32_t sector = fs.fat_start + (n * 2) / 512;
    uint8_t buf[512];
    ata_read_sector(sector, fs.drive, buf);
    return *(uint16_t*) (buf + (n * 2) % 512);
}

void fat_set(uint32_t n, uint16_t value) {
    uint32_t sector = (n * 2) / 512;
    uint16_t offset = (n * 2) % 512;
    char buf[512];

    for(size_t i = 0; i < fs.num_fats; i++) {
        uint32_t fat_sec = fs.fat_start + i * fs.sectors_per_fat + sector;
        ata_read_sector(fat_sec, fs.drive, buf);
        *(uint16_t*) (buf + offset) = value;
        ata_write_sector(fat_sec, fs.drive, buf);
    }
}

uint8_t dir_read_sector(vfs_node_t* node, size_t index, uint8_t* buf, uint32_t* out) {
    if(node->inode == 0) {
        if(index >= fs.root_sectors) {
            return 0;
        }
        ata_read_sector(fs.root_start + index, fs.drive, buf);
        return 1;
    }

    size_t cluster_idx = index / fs.sectors_per_cluster;
    size_t sec_in_cluster = index % fs.sectors_per_cluster;

    uint32_t cluster = node->inode;
    for(size_t k = 0; k < cluster_idx; k++) {
        cluster = fat_lookup(cluster);
        if(cluster >= 0xFFF8) {
            return 0;
        }
    }
    *out = fs.data_start + (cluster - 2) * fs.sectors_per_cluster + sec_in_cluster;
    ata_read_sector(*out, fs.drive, buf);
    return 1;
}

char* fat16_read(vfs_node_t* node, size_t offset, size_t size, char* buf) {
    uint32_t cluster = node->inode;
    size_t read = 0;
    uint8_t tmp_buf[512];

    while(cluster < 0xFFF8 && read < size) {
        uint32_t lba = fs.data_start + (cluster - 2) * fs.sectors_per_cluster;
        for(size_t s = 0; s < fs.sectors_per_cluster; s++) {
            ata_read_sector(lba + s, fs.drive, tmp_buf);
            size_t remaining = size - read;
            size_t n = remaining < 512 ? remaining : 512;
            memcpy(buf + read, tmp_buf, n);
            read += n;
            if(read >= size) {
                return buf;
            }
        }
        cluster = fat_lookup(cluster);
        if(cluster == 0x0) {
            return NULL;
        }
    }
    return buf;
}

void* fat16_write(vfs_node_t* node, size_t offset, size_t size, char* buf) {
    if(node->inode == 0) {
        for(size_t c = 2; c < fs.sectors_per_fat * 256; c++) {
            uint32_t new_cluster = fat_lookup(c);
            if(new_cluster == 0) {
                fat_set(c, 0xFFFF);
                node->inode = c;
                break;
            }
        }
    }
    uint32_t cluster = node->inode;
    size_t written = 0;
    uint8_t tmp_buf[512];

    while(written < size) {
        uint32_t lba = fs.data_start + (cluster - 2) * fs.sectors_per_cluster;
        for(size_t s = 0; s < fs.sectors_per_cluster; s++) {
            uint16_t n = size - written < 512 ? size - written : 512;
            if(size - written < 512) {
                ata_read_sector(lba + s, fs.drive, tmp_buf);
                memcpy(tmp_buf, buf + written, size - written);
            } else {
                memcpy(tmp_buf, buf + written, 512);
            }
            ata_write_sector(lba + s, fs.drive, tmp_buf);
            written += n;
            if(written >= size) {
                uint8_t dir_buf[512];
                fat16_node_t* fn = (fat16_node_t*) node->impl;
                ata_read_sector(fn->dir_lba, fs.drive, dir_buf);
                fat16_dirent_t* entry = (fat16_dirent_t*) (dir_buf + fn->dir_off);
                entry->file_size = written;
                entry->inode = node->inode;
                ata_write_sector(fn->dir_lba, fs.drive, dir_buf);
                return (void*) 0x1;
            }
        }
        uint32_t old_cluster = cluster;
        cluster = fat_lookup(cluster);
        if(cluster == 0x0) {
            return NULL;
        }
        if(cluster >= 0xFFF8) {
            for(size_t c = 2; c < fs.sectors_per_fat * 256; c++) {
                uint32_t new_cluster = fat_lookup(c);
                if(new_cluster == 0) {
                    fat_set(old_cluster, c);
                    fat_set(c, 0xFFFF);
                    cluster = c;
                    break;
                }
                if(c < fs.sectors_per_fat * 256 - 1) {
                    return NULL;
                }
            }
        }
    }
}

vfs_node_t* fat16_open(vfs_node_t* node) {
    return node;
}

void* fat16_close(vfs_node_t* node) {
    free(node->impl);
    free(node);
}

dirent_t* fat16_readdir(vfs_node_t* node, size_t index) {
    size_t valid = 0;
    uint8_t buf[512];
    uint32_t dir_lba;
    for(size_t i = 0; dir_read_sector(node, i, buf, &dir_lba); i++) {
        for(size_t e = 0; e < 16; e++) {
            fat16_dirent_t* entry = (fat16_dirent_t*) (buf + e * 32);
            if(entry->name[0] == 0x00) {
                return NULL;
            }
            if(entry->name[0] == 0xE5 || entry->attr == 0x0F || (entry->attr & 0x08)) {
                continue;
            }

            if(valid == index) {
                char filename[13];
                size_t k = 0;
                for(size_t l = 0; l < 8; l++) {
                    if(entry->name[l] == ' ') {
                        break;
                    }
                    filename[k++] = entry->name[l];
                }
                if(entry->name[8] != ' ') {
                    filename[k++] = '.';
                    for(size_t j = 8; j < 11; j++) {
                        if(entry->name[j] == ' ') {
                            break;
                        }
                        filename[k++] = entry->name[j];
                    }
                }
                filename[k] = '\0';

                dirent_t* dir = (dirent_t*) malloc(sizeof(dirent_t));

                strlcpy(dir->name, filename, sizeof(dir->name));
                dir->inode = entry->inode;
                dir->flags = entry->attr & 0x10 ? VFS_DIRECTORY : VFS_FILE;
                return dir;
            }
            valid++;
        }
    }

    return NULL;
}

vfs_node_t* fat16_finddir(vfs_node_t* node, char* name) {
    size_t valid = 0;
    uint8_t buf[512];
    uint32_t dir_lba;
    for(size_t i = 0; dir_read_sector(node, i, buf, &dir_lba); i++) {
        for(size_t e = 0; e < 16; e++) {
            fat16_dirent_t* entry = (fat16_dirent_t*) (buf + e * 32);
            if(entry->name[0] == 0x00) {
                return NULL;
            }
            if(entry->name[0] == 0xE5 || entry->attr == 0x0F || (entry->attr & 0x08)) {
                continue;
            }

            char filename[13];
            size_t k = 0;
            for(size_t l = 0; l < 8; l++) {
                if(entry->name[l] == ' ') {
                    break;
                }
                filename[k++] = entry->name[l];
            }
            if(entry->name[8] != ' ') {
                filename[k++] = '.';
                for(size_t j = 8; j < 11; j++) {
                    if(entry->name[j] == ' ') {
                        break;
                    }
                    filename[k++] = entry->name[j];
                }
            }
            filename[k] = '\0';

            if(strcmp(filename, name) == 0) {
                vfs_node_t* found = (vfs_node_t*) malloc(sizeof(vfs_node_t));
                strlcpy(found->name, filename, sizeof(found->name));
                found->flags = (entry->attr & 0x10) ? VFS_DIRECTORY : VFS_FILE;
                found->size = entry->file_size;
                found->inode = entry->inode;
                fat16_node_t* fn = malloc(sizeof(fat16_node_t));
                fn->dir_lba = dir_lba;
                fn->dir_off = e * 32;
                found->impl = fn;
                found->read = fat16_read;
                found->write = fat16_write;
                found->open = fat16_open;
                found->close = fat16_close;
                found->readdir = fat16_readdir;
                found->finddir = fat16_finddir;
                found->create = fat16_create;
                found->delete = fat16_delete;
                return found;
            }
        }
    }

    return NULL;
}

vfs_node_t* fat16_create(vfs_node_t* dir, char* name, uint32_t flags) {
    char filename[11];
    memset(filename, ' ', 11);
    size_t dot = strlen(name);
    for(size_t i = 0; i < strlen(name); i++) {
        if(name[i] == '.') {
            dot = i;
            break;
        }
    }
    size_t base_len = dot > 8 ? 8 : dot;

    for(size_t i = 0; i < base_len; i++) {
        filename[i] = toupper(name[i]);
    }

    if(dot < strlen(name)) {
        char* ext = name + dot + 1;
        size_t ext_len = 3 > strlen(ext) ? strlen(ext) : 3;
        for(size_t i = 0; i < ext_len; i++) {
            filename[8 + i] = toupper(ext[i]);
        }
    }

    uint16_t dir_off;
    uint8_t buf[512];
    uint32_t dir_lba;
    for(size_t i = 0; dir_read_sector(dir, i, buf, &dir_lba); i++) {
        for(size_t e = 0; e < 16; e++) {
            fat16_dirent_t* entry = (fat16_dirent_t*) (buf + e * 32);
            if(entry->name[0] == 0xE5 || entry->name[0] == 0x00) {
                dir_off = e * 32;

                memset(entry, 0, 32);
                memcpy(entry->name, filename, 11);
                entry->attr = flags == VFS_DIRECTORY ? 0x10 : 0x20;
                if(flags == VFS_DIRECTORY) {
                    for(size_t c = 2; c < fs.sectors_per_fat * 256; c++) {
                        uint32_t new_cluster = fat_lookup(c);
                        if(new_cluster == 0) {
                            fat_set(c, 0xFFF8);
                            entry->inode = c;
                            break;
                        }
                    }
                } else {
                    entry->inode = 0;
                }
                entry->file_size = 0;

                fat16_dirent_t* new_entry = (fat16_dirent_t*) (buf + dir_off);
                memmove(new_entry, entry, sizeof(fat16_dirent_t));
                ata_write_sector(dir_lba, fs.drive, buf);

                vfs_node_t* node = malloc(sizeof(vfs_node_t));
                if(!node) {
                    return NULL;
                }
                strlcpy(node->name, name, VFS_NAME_MAX);
                node->flags = flags;
                node->size = 0;
                node->inode = 0;
                fat16_node_t* fat_node = malloc(sizeof(fat16_node_t));
                fat_node->dir_lba = dir_lba;
                fat_node->dir_off = dir_off;
                node->impl = fat_node;
                node->read = fat16_read;
                node->write = fat16_write;
                node->open = fat16_open;
                node->close = fat16_close;
                node->readdir = fat16_readdir;
                node->finddir = fat16_finddir;
                node->create = fat16_create;
                node->delete = fat16_delete;

                return node;
            }
        }
    }

    return NULL;
}

void* fat16_delete(vfs_node_t* node) {
    if(node->inode != 0) {
        uint32_t cluster = node->inode;
        while(cluster >= 2 && cluster < 0xFFF8) {
            uint32_t next = fat_lookup(cluster);
            fat_set(cluster, 0x0000);
            cluster = next;
        }
    }

    uint8_t tmp_buf[512];
    fat16_node_t* fn = (fat16_node_t*) node->impl;
    ata_read_sector(fn->dir_lba, fs.drive, tmp_buf);
    fat16_dirent_t* entry = (fat16_dirent_t*) (tmp_buf + fn->dir_off);
    entry->name[0] = 0xE5;
    ata_write_sector(fn->dir_lba, fs.drive, tmp_buf);

    return (uint8_t*) 1;
}
