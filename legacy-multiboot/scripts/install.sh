#!/bin/bash
set -e

sudo cp kernel.bin /mnt/boot/kernel
sync

echo "Kernel installed"