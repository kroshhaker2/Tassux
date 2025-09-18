#ifndef VFS_H
#define VFS_H

#include <stdint.h>

typedef struct vfs_node_t {
    char name[128];
    uint32_t inode;
    uint32_t size;
    uint8_t type;
    struct vfs_node_t *next;
} vfs_node_t;

typedef struct filesystem_driver {
    const char *name;
    int (*mount)(uint32_t lba);
    int (*read)(struct vfs_node_t *node, uint32_t offset, uint32_t size, void *buffer);
    int (*readdir)(struct vfs_node_t *node, uint32_t index, struct vfs_node_t *out);
} filesystem_driver_t;

void vfs_register(filesystem_driver_t *driver);
int vfs_mount(uint32_t lba, const char *fsname);
struct vfs_node_t *vfs_open(const char *path);

#endif
