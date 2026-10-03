#!/bin/bash
set -e

IMAGE="mydisk.img"

mkdir -p /mnt/boot/efi

LOOP=$(sudo losetup -fP --show "$IMAGE")

echo "$LOOP" > .loopdev

sudo mount -o sync "${LOOP}p2" /mnt
sudo mount -o sync "${LOOP}p1" /mnt/boot/efi

echo "Mounted on $LOOP"