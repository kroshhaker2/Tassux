losetup -fP mydisk.img
mount -o sync /dev/loop0p2 /mnt
mount -o sync /dev/loop0p1 /mnt/boot
