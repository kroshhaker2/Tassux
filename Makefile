CC = gcc
ASM = nasm
RUSTC = rustc
LD = ld

CFLAGS = -ffreestanding -fno-builtin -nostdlib -nostdinc -std=gnu99 \
         -fno-stack-protector -m32 -Iinclude -Ikernel -Iboot
CFLAGS_DEBUG = -ffreestanding -fno-builtin -nostdlib -nostdinc \
               -fno-stack-protector -m32 -Iinclude -Ikernel -Iboot -O0 -g -DDEBUG
ASMFLAGS = -f elf32
RUSTFLAGS = --target=i386-unknown-none -C opt-level=2 -C relocation-model=static \
             -C link-arg=-nostdlib --crate-type=staticlib
LDFLAGS = -T link.ld -nostdlib -m elf_i386

# Исходники
C_SOURCES := $(shell find kernel boot -name '*.c')
ASM_SOURCES := $(shell find kernel boot -name '*.asm')
RS_SOURCES := $(shell find kernel -name '*.rs')

# Объектные файлы
C_OBJECTS := $(patsubst %.c, build/%.o, $(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm, build/%_asm.o, $(ASM_SOURCES))
RS_OBJECTS := $(patsubst %.rs, build/%.o, $(RS_SOURCES))

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS) $(RS_OBJECTS)

all: kernel.bin

debug: CFLAGS := $(CFLAGS_DEBUG)
debug: clean kernel.bin

# C
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ASM
build/%_asm.o: %.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

# Rust
build/%.o: %.rs
	@mkdir -p $(dir $@)
	$(RUSTC) $(RUSTFLAGS) $< -o $@

# Линковка
kernel.bin: $(OBJECTS) link.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

run:
	bash scripts/start.sh

clean:
	rm -rf build *.bin

mount:
	./scripts/mount.sh

umount:
	./scripts/umount.sh

install:
	./scripts/install.sh

deploy: kernel.bin
	./scripts/mount.sh
	./scripts/install.sh
	./scripts/umount.sh

watch:
	find kernel boot include -type f | entr -r make deploy

.PHONY: all run clean install debug
