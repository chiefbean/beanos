#ifndef _KERNEL_VFS_H
#define _KERNEL_VFS_H 1

#include <stdint.h>
#include <stddef.h>

#define VFS_NAME_MAX 128 

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VFS_FILE,
    VFS_DIRECTORY
} Flags;

typedef struct {
    char name[VFS_NAME_MAX];
    uint32_t inode;
    Flags flags;
} dirent_t;

typedef struct vfs_node {
    char name[VFS_NAME_MAX];
    Flags flags;
    uint32_t size;
    uint32_t inode;
    void* impl;
    char* (*read)(struct vfs_node* node, size_t offset, size_t size, char* buf);
    void* (*write)(struct vfs_node* node, size_t offset, size_t size, char* buf);
    struct vfs_node* (*open)(struct vfs_node* node);
    void* (*close)(struct vfs_node* node);
    dirent_t* (*readdir)(struct vfs_node* node, size_t index);
    struct vfs_node* (*finddir)(struct vfs_node* node, char* name);
    struct vfs_node* (*create)(struct vfs_node* dir, char* name, uint32_t flags);
    void* (*delete)(struct vfs_node* node);
} vfs_node_t;

char* vfs_read(vfs_node_t* node, size_t offset, size_t size, char* buf);
void* vfs_write(vfs_node_t* node, size_t offset, size_t size, char* buf);
vfs_node_t* vfs_open(vfs_node_t* node);
void* vfs_close(vfs_node_t* node);
dirent_t* vfs_readdir(vfs_node_t* node, size_t index);
vfs_node_t* vfs_finddir(vfs_node_t* node, char* name);
vfs_node_t* vfs_create(struct vfs_node* dir, char* name, uint32_t flags);
void* vfs_delete(vfs_node_t* node);

extern vfs_node_t* vfs_root;

#ifdef __cplusplus
}
#endif

#endif