#ifndef VFS_H
#define VFS_H

#include "../include/stddef.h"
#include "../include/stdint.h"

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

struct file;
struct inode;
struct dirent;

struct filesystem {
  const char *name;
  uint32_t signature_offset;
  uint8_t signature[4];
  uint8_t sig_len;
};

#define FS_COUNT (sizeof(filesystems) / sizeof(filesystems[0]))

struct file_ops {
  int (*read)(struct file *, void *, size_t);
  int (*write)(struct file *, const void *, size_t);
  int (*open)(struct inode *);
  int (*close)(struct inode *);
  int (*readdir)(struct file *, struct dirent *);
};

extern void detect_filesystem(uint32_t start_lba);

#endif
