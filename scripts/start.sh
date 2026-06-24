#!/bin/bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DISK_IMAGE="$PROJECT_ROOT/mydisk.img"
UEFI_DIR="$PROJECT_ROOT/uefi"

OVMF_CODE="${OVMF_CODE:-/usr/share/edk2/x64/OVMF_CODE.4m.fd}"
OVMF_VARS_TEMPLATE="${OVMF_VARS_TEMPLATE:-/usr/share/edk2/x64/OVMF_VARS.4m.fd}"
OVMF_VARS="$UEFI_DIR/OVMF_VARS.4m.fd"

if [[ ! -f "$OVMF_CODE" ]]; then
    echo "OVMF code file not found: $OVMF_CODE" >&2
    echo "Install edk2-ovmf or set OVMF_CODE=/path/to/OVMF_CODE.4m.fd" >&2
    exit 1
fi

if [[ ! -f "$OVMF_VARS_TEMPLATE" ]]; then
    echo "OVMF vars template not found: $OVMF_VARS_TEMPLATE" >&2
    echo "Install edk2-ovmf or set OVMF_VARS_TEMPLATE=/path/to/OVMF_VARS.4m.fd" >&2
    exit 1
fi

if [[ ! -f "$DISK_IMAGE" ]]; then
    echo "Disk image not found: $DISK_IMAGE" >&2
    echo "Create it with: qemu-img create -f raw mydisk.img 1G" >&2
    exit 1
fi

mkdir -p "$UEFI_DIR"

if [[ ! -f "$OVMF_VARS" ]]; then
    cp "$OVMF_VARS_TEMPLATE" "$OVMF_VARS"
fi

QEMU_ACCEL=(-enable-kvm -cpu host)
if [[ ! -r /dev/kvm || ! -w /dev/kvm ]]; then
    echo "KVM is not available, falling back to TCG emulation." >&2
    QEMU_ACCEL=(-accel tcg -cpu qemu64)
fi

qemu-system-x86_64 \
    "${QEMU_ACCEL[@]}" \
    -machine q35 \
    -smp 4 \
    -m 1G \
    \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,file="$OVMF_VARS" \
    \
    -drive id=disk0,file="$DISK_IMAGE",format=raw,if=none \
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
