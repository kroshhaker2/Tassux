#!/bin/bash
set -e

qemu-system-x86_64 \
    -enable-kvm \
    -cpu host \
    -machine q35 \
    -smp 4 \
    -m 1G \
    \
    -drive if=pflash,format=raw,readonly=on,file=uefi/OVMF_CODE.4m.fd \
    -drive if=pflash,format=raw,file=uefi/OVMF_VARS.4m.fd \
    \
    -drive id=disk0,file=mydisk.img,format=raw,if=none \
    -device ahci,id=ahci \
    -device ide-hd,drive=disk0,bus=ahci.0 \
    \
    -vga std \
    \
    -device qemu-xhci \
    -device usb-kbd \
    -device usb-mouse \
    \
    -display sdl \
    -serial stdio
