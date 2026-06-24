#!/bin/bash
set -e

cp kernel.bin /mnt/boot/kernel
sync

echo "Kernel installed"