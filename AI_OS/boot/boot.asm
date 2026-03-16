; =============================================================================
; AI_OS — Bootloader Entry Point
; File:    boot/boot.asm
; Arch:    x86_64
; Format:  Multiboot2 (совместим с GRUB2 и VMware)
; Desc:    Устанавливает Multiboot2 заголовок, инициализирует стек,
;          передаёт управление kernel_main в C++
; =============================================================================

; --- Multiboot2 константы ---
MULTIBOOT2_MAGIC    equ 0xE85250D6
MULTIBOOT2_ARCH     equ 0               ; i386 (protected mode)

; --- Размер стека ядра: 16 KiB ---
KERNEL_STACK_SIZE   equ 0x4000

; =============================================================================
; Секция .multiboot2 — ДОЛЖНА быть первой в бинарнике (linker.ld гарантирует)
; =============================================================================
section .multiboot2
align 8

header_start:
    dd MULTIBOOT2_MAGIC
    dd MULTIBOOT2_ARCH
    dd (header_end - header_start)                          ; длина заголовка
    dd -(MULTIBOOT2_MAGIC + MULTIBOOT2_ARCH + (header_end - header_start))  ; checksum

    ; --- Тег: информация о памяти ---
    align 8
    dw 1        ; type: boot command line (запрашиваем)
    dw 0        ; flags
    dd 8        ; size

    ; --- Завершающий тег (обязателен) ---
    align 8
    dw 0        ; type = 0 → конец списка тегов
    dw 0
    dd 8
header_end:

; =============================================================================
; BSS — стек ядра (не инициализируется, экономим место в бинарнике)
; =============================================================================
section .bss
align 16

global stack_bottom
global stack_top

stack_bottom:
    resb KERNEL_STACK_SIZE
stack_top:

; =============================================================================
; Точка входа — GRUB передаёт управление сюда в 32-bit protected mode
; eax = 0x36D76289 (Multiboot2 magic number для подтверждения)
; ebx = физический адрес структуры multiboot2_info
; =============================================================================
section .text
bits 32

global _start
extern kernel_main          ; определён в kernel/kernel.cpp

_start:
    ; 1. Установить стек ядра
    mov     esp, stack_top

    ; 2. Очистить флаги (EFLAGS = 0)
    push    0
    popf

    ; 3. Передать аргументы в kernel_main(uint32_t magic, uint32_t mbi_addr)
    ;    cdecl: аргументы справа налево через стек
    push    ebx             ; arg2: адрес multiboot2 info структуры
    push    eax             ; arg1: magic (0x36D76289 если загружен через GRUB2)

    ; 4. Вызов ядра
    call    kernel_main

    ; 5. Ядро никогда не должно возвращаться — зависаем
    cli                     ; запретить прерывания
.halt:
    hlt                     ; процессор в режим ожидания
    jmp     .halt           ; если NMI разбудит — снова hlt
