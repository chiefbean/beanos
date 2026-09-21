#include <kernel/vfs.h>

vfs_node_t* vfs_root;

char* vfs_read(vfs_node_t* node, size_t offset, size_t size, char* buf) {
    if (node && node->read) return node->read(node, offset, size, buf);
    return NULL;
}

void* vfs_write(vfs_node_t* node, size_t offset, size_t size, char* buf) {
    if (node && node->write) return node->write(node, offset, size, buf);
    return NULL;
}

vfs_node_t* vfs_open(vfs_node_t* node) {
    if (node && node->open) return node->open(node);
    return NULL;
}

void* vfs_close(vfs_node_t* node) {
    if (node && node->close) return node->close(node);
    return NULL;
}

dirent_t* vfs_readdir(vfs_node_t* node, size_t index) {
    if (node && node->readdir) return node->readdir(node, index);
    return NULL;
}   

vfs_node_t* vfs_finddir(vfs_node_t* node, char* name) {
    if (node && node->finddir) return node->finddir(node, name);
    return NULL;
}

vfs_node_t* vfs_create(vfs_node_t* dir, char* name, uint32_t flags) {
    if (dir && dir->create) return dir->create(dir, name, flags);
    return NULL;
}

void* vfs_delete(vfs_node_t* node) {
    if (node && node->delete) return node->delete(node);
    return NULL;
}