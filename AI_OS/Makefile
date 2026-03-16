# =============================================================================
# AI_OS — Система сборки
# File:  Makefile
# Desc:  Компиляция ядра, линковка, создание ISO-образа для VMware/QEMU
#
# ТРЕБОВАНИЯ (Arch Linux):
#   sudo pacman -S nasm grub xorriso i686-elf-gcc
#   Либо: yay -S cross-i686-elf-gcc  (если нет в репо)
#
#   Для x86_64 cross-compiler:
#   sudo pacman -S x86_64-elf-gcc x86_64-elf-binutils
# =============================================================================

# --- Кросс-компилятор (x86_64-elf для bare-metal) ---
# Если нет x86_64-elf-gcc, можно использовать i686-elf-gcc на первом этапе
CROSS   ?= i686-elf

CC      := $(CROSS)-gcc
CXX     := $(CROSS)-g++
LD      := $(CROSS)-ld
NASM    := nasm
GRUB    := grub-mkrescue

# --- Директории ---
BOOT_DIR    := boot
KERNEL_DIR  := kernel
BUILD_DIR   := build
ISO_DIR     := iso

# --- Выходные файлы ---
KERNEL_ELF  := $(BUILD_DIR)/aios_kernel.elf
ISO_FILE    := $(BUILD_DIR)/aios.iso

# --- Флаги компилятора ---
# -ffreestanding    : нет стандартной библиотеки
# -fno-exceptions   : нет C++ exceptions (нужен ABI для них)
# -fno-rtti         : нет RTTI
# -nostdlib         : не линковать стандартные библиотеки
# -O2               : оптимизация
CXXFLAGS := \
    -std=c++20          \
    -ffreestanding      \
    -fno-exceptions     \
    -fno-rtti           \
    -fno-stack-protector\
    -nostdlib           \
    -Wall               \
    -Wextra             \
    -O2                 \
    -I$(KERNEL_DIR)

CFLAGS := \
    -std=c11            \
    -ffreestanding      \
    -fno-stack-protector\
    -nostdlib           \
    -Wall               \
    -O2                 \
    -I$(KERNEL_DIR)

NASMFLAGS := -f elf32

LDFLAGS := \
    -T $(KERNEL_DIR)/linker.ld  \
    -nostdlib

# --- Исходные файлы ---
BOOT_SOURCES  := $(BOOT_DIR)/boot.asm

KERNEL_SOURCES := \
    $(KERNEL_DIR)/kernel.cpp                    \
    $(KERNEL_DIR)/drivers/terminal/terminal.cpp

# --- Объектные файлы ---
BOOT_OBJ    := $(BUILD_DIR)/boot.o
KERNEL_OBJS := $(patsubst $(KERNEL_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(KERNEL_SOURCES))

# =============================================================================
# Цели
# =============================================================================

.PHONY: all clean iso run-qemu help

# --- По умолчанию: собрать всё + ISO ---
all: $(ISO_FILE)
	@echo ""
	@echo "  ╔══════════════════════════════════════╗"
	@echo "  ║   AI_OS build complete!              ║"
	@echo "  ║   ISO: $(ISO_FILE)        ║"
	@echo "  ╚══════════════════════════════════════╝"
	@echo ""

# --- Создать ISO образ ---
$(ISO_FILE): $(KERNEL_ELF)
	@echo "[ISO] Creating bootable ISO..."
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/aios_kernel.elf
	@$(GRUB) -o $(ISO_FILE) $(ISO_DIR)
	@echo "[ISO] Done: $(ISO_FILE)"

# --- Слинковать ядро ---
$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJS) | $(BUILD_DIR)
	@echo "[LD ] Linking kernel ELF..."
	@$(LD) $(LDFLAGS) -o $@ $^
	@echo "[LD ] $(KERNEL_ELF) OK"

# --- Скомпилировать bootloader (NASM) ---
$(BOOT_OBJ): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	@echo "[ASM] $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

# --- Скомпилировать .cpp файлы ---
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "[C++] $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Создать директорию build ---
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# --- Запустить в QEMU (если установлен) ---
run-qemu: $(ISO_FILE)
	@echo "[QEMU] Launching AI_OS..."
	qemu-system-x86_64              \
	    -cdrom $(ISO_FILE)          \
	    -m 256M                     \
	    -vga std                    \
	    -no-reboot                  \
	    -serial stdio

# --- Запустить в QEMU с отладчиком GDB ---
debug: $(ISO_FILE)
	@echo "[QEMU] Debug mode — waiting for GDB on :1234"
	qemu-system-x86_64              \
	    -cdrom $(ISO_FILE)          \
	    -m 256M                     \
	    -vga std                    \
	    -no-reboot                  \
	    -s -S

# --- Очистить артефакты сборки ---
clean:
	@echo "[CLN] Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(ISO_DIR)/boot/aios_kernel.elf
	@echo "[CLN] Done."

# --- Помощь ---
help:
	@echo ""
	@echo "  AI_OS Build System"
	@echo "  =================="
	@echo "  make          — собрать ядро + ISO"
	@echo "  make run-qemu — запустить в QEMU"
	@echo "  make debug    — QEMU + GDB сервер"
	@echo "  make clean    — очистить build/"
	@echo ""
	@echo "  Требования:"
	@echo "  sudo pacman -S nasm grub xorriso i686-elf-gcc"
	@echo ""
