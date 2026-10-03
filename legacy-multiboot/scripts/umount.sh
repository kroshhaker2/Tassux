#!/bin/bash
set -e

LOOP=$(cat .loopdev)

sync

sudo umount /mnt/boot/efi || true
sudo umount /mnt || true

sudo losetup -d "$LOOP"

rm -f .loopdev

echo "Unmounted $LOOP"