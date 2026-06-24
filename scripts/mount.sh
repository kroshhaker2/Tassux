#!/bin/bash
set -e

IMAGE="mydisk.img"

mkdir -p /mnt/boot/efi

LOOP=$(losetup -fP --show "$IMAGE")

echo "$LOOP" > .loopdev

mount -o sync "${LOOP}p2" /mnt
mount -o sync "${LOOP}p1" /mnt/boot/efi

echo "Mounted on $LOOP"