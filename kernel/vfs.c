#include "vfs.h"

#include "ext2.h"
#include "console.h"
#include <stdint.h>

#define MAX_FS 8
static filesystem_driver_t *drivers[MAX_FS];
static int driver_count = 0;

void vfs_register(filesystem_driver_t *driver) {
    if (driver_count < MAX_FS) {
        drivers[driver_count++] = driver;
    }
}

int vfs_mount(uint32_t lba, const char *fsname) {
    for (int i = 0; i < driver_count; i++) {
        if (!strcmp(drivers[i]->name, fsname)) {
            return drivers[i]->mount(lba);
        }
    }
    print("No such FS driver\n");
    return -1;
}
