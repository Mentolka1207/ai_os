// =============================================================================
// AI_OS — Главная точка входа ядра
// File:  kernel/kernel.cpp
// Arch:  x86_64 (загружается в 32-bit protected mode через Multiboot2)
// =============================================================================

#include <stdint.h>
#include <stddef.h>
#include "drivers/terminal/terminal.h"

// =============================================================================
// Multiboot2 Magic Number (GRUB передаёт в eax при загрузке)
// =============================================================================
static constexpr uint32_t MULTIBOOT2_BOOTLOADER_MAGIC = 0x36D76289;

// =============================================================================
// Версия ядра
// =============================================================================
static constexpr const char* AI_OS_VERSION = "0.0.1-prealpha";
static constexpr const char* AI_OS_ARCH    = "x86_64";
static constexpr const char* AI_OS_BUILD   = __DATE__ " " __TIME__;

// =============================================================================
// Вспомогательные функции вывода
// =============================================================================
static void print_separator(char c = '-', int len = 78) {
    for (int i = 0; i < len; i++) Terminal::put_char(c);
    Terminal::put_char('\n');
}

static void print_kv(const char* key, const char* value,
                     VGAColor key_color   = VGAColor::Cyan,
                     VGAColor value_color = VGAColor::White) {
    Terminal::set_color(key_color, VGAColor::Black);
    Terminal::write("  ");
    Terminal::write(key);
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write(" ......... ");
    Terminal::set_color(value_color, VGAColor::Black);
    Terminal::writeln(value);
}

static void print_ok(const char* msg) {
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("  [ ");
    Terminal::set_color(VGAColor::LightGreen, VGAColor::Black);
    Terminal::write("OK");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write(" ] ");
    Terminal::set_color(VGAColor::LightGrey, VGAColor::Black);
    Terminal::writeln(msg);
}

static void print_warn(const char* msg) {
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("  [");
    Terminal::set_color(VGAColor::Yellow, VGAColor::Black);
    Terminal::write("WARN");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("] ");
    Terminal::set_color(VGAColor::Yellow, VGAColor::Black);
    Terminal::writeln(msg);
}

static void print_fail(const char* msg) {
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("  [");
    Terminal::set_color(VGAColor::LightRed, VGAColor::Black);
    Terminal::write("FAIL");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("] ");
    Terminal::set_color(VGAColor::LightRed, VGAColor::Black);
    Terminal::writeln(msg);
}

// =============================================================================
// Проверка Multiboot2
// =============================================================================
static bool check_multiboot(uint32_t magic) {
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

// =============================================================================
// Вывод ASCII-логотипа AI_OS
// =============================================================================
static void print_logo() {
    Terminal::set_color(VGAColor::LightCyan, VGAColor::Black);
    Terminal::writeln("   ___    ____    ___    ____  ");
    Terminal::writeln("  / _ |  /  _/   / _ |  / __/");
    Terminal::writeln(" / __ | _/ /    / __ | _\\ \\  ");
    Terminal::writeln("/_/ |_|/___/   /_/ |_|/___/  ");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::writeln("                               Artificial Intelligence OS");
    Terminal::put_char('\n');
}

// =============================================================================
// kernel_main — вызывается из boot.asm
// =============================================================================
extern "C" void kernel_main(uint32_t magic, uint32_t mbi_addr) {

    // --- 1. Инициализировать терминал (первое что делаем) ---
    Terminal::init();

    // --- 2. Логотип ---
    print_logo();

    // --- 3. Шапка ---
    Terminal::set_color(VGAColor::LightGreen, VGAColor::Black);
    Terminal::writeln("  AI_OS Kernel Booting...");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    print_separator('=');
    Terminal::put_char('\n');

    // --- 4. Системная информация ---
    Terminal::set_color(VGAColor::Yellow, VGAColor::Black);
    Terminal::writeln("  SYSTEM INFO");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    print_separator();

    print_kv("OS Name",      "AI_OS");
    print_kv("Version",      AI_OS_VERSION);
    print_kv("Architecture", AI_OS_ARCH);
    print_kv("Build Date",   AI_OS_BUILD);
    print_kv("Bootloader",   "GRUB2 + Multiboot2");

    // Вывести адрес Multiboot2 info
    Terminal::set_color(VGAColor::Cyan, VGAColor::Black);
    Terminal::write("  MBI Address");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write(" ......... ");
    Terminal::set_color(VGAColor::White, VGAColor::Black);
    Terminal::write_hex(mbi_addr);
    Terminal::put_char('\n');

    Terminal::put_char('\n');

    // --- 5. Инициализация подсистем ---
    Terminal::set_color(VGAColor::Yellow, VGAColor::Black);
    Terminal::writeln("  KERNEL SUBSYSTEMS");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    print_separator();

    // Проверка загрузчика
    if (check_multiboot(magic)) {
        print_ok("Multiboot2 signature verified");
    } else {
        print_fail("Multiboot2 signature INVALID — unknown bootloader");
    }

    print_ok("VGA Terminal Driver (80x25) initialized");
    print_ok("Kernel stack established (16 KiB)");
    print_warn("Memory Manager (PMM/VMM) — NOT YET IMPLEMENTED");
    print_warn("Interrupt Descriptor Table (IDT) — NOT YET IMPLEMENTED");
    print_warn("Process Scheduler — NOT YET IMPLEMENTED");
    print_warn("File System (AIFS) — NOT YET IMPLEMENTED");
    print_warn("Network Stack — NOT YET IMPLEMENTED");

    Terminal::put_char('\n');

    // --- 6. Готово ---
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    print_separator('=');
    Terminal::set_color(VGAColor::LightGreen, VGAColor::Black);
    Terminal::writeln("  AI_OS kernel v" AI_OS_VERSION " loaded successfully.");
    Terminal::set_color(VGAColor::LightCyan, VGAColor::Black);
    Terminal::writeln("  Next: implement IDT + Physical Memory Manager.");
    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::writeln("  System halted. Press any key... (just kidding, no input yet)");
    Terminal::put_char('\n');

    Terminal::set_color(VGAColor::DarkGrey, VGAColor::Black);
    Terminal::write("  Magic: ");
    Terminal::write_hex(magic);
    Terminal::write("   MBI: ");
    Terminal::write_hex(mbi_addr);
    Terminal::put_char('\n');

    // --- 7. Бесконечный цикл (ядро не возвращает управление) ---
    for (;;)
        asm volatile("hlt");
}
