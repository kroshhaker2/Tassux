#!/bin/bash
set -e

LOOP=$(cat .loopdev)

sync

umount /mnt/boot/efi || true
umount /mnt || true

losetup -d "$LOOP"

rm -f .loopdev

echo "Unmounted $LOOP"