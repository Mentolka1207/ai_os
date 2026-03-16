#pragma once
// =============================================================================
// AI_OS — VGA Text Mode Terminal Driver
// File:  kernel/drivers/terminal/terminal.h
// Desc:  80x25 VGA текстовый режим (0xB8000).
//        Цвета, прокрутка, вывод строк/символов/чисел.
// =============================================================================

#include <stdint.h>
#include <stddef.h>

// --- VGA константы ---
static constexpr uint16_t VGA_WIDTH  = 80;
static constexpr uint16_t VGA_HEIGHT = 25;
static constexpr uint32_t VGA_MEMORY = 0xB8000;

// --- VGA цвета ---
enum class VGAColor : uint8_t {
    Black        = 0,
    Blue         = 1,
    Green        = 2,
    Cyan         = 3,
    Red          = 4,
    Magenta      = 5,
    Brown        = 6,
    LightGrey    = 7,
    DarkGrey     = 8,
    LightBlue    = 9,
    LightGreen   = 10,
    LightCyan    = 11,
    LightRed     = 12,
    LightMagenta = 13,
    Yellow       = 14,
    White        = 15,
};

// Упаковать fg+bg в один байт атрибута VGA
static inline uint8_t vga_entry_color(VGAColor fg, VGAColor bg) {
    return static_cast<uint8_t>(fg) | (static_cast<uint8_t>(bg) << 4);
}

// Упаковать символ + атрибут в 16-bit VGA entry
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
}

// =============================================================================
// Класс Terminal
// =============================================================================
class Terminal {
public:
    // --- Инициализация: очистить экран, установить цвет по умолчанию ---
    static void init();

    // --- Установить цвет текста ---
    static void set_color(VGAColor fg, VGAColor bg);

    // --- Вывод одного символа ---
    static void put_char(char c);

    // --- Вывод строки (null-terminated) ---
    static void write(const char* str);

    // --- Вывод строки + перевод строки ---
    static void writeln(const char* str);

    // --- Вывод числа (hex) ---
    static void write_hex(uint32_t value);

    // --- Вывод числа (decimal) ---
    static void write_dec(uint32_t value);

    // --- Очистить экран ---
    static void clear();

    // --- Установить позицию курсора ---
    static void set_cursor(uint16_t col, uint16_t row);

private:
    static uint16_t  row_;
    static uint16_t  col_;
    static uint8_t   color_;
    static uint16_t* buffer_;

    static void scroll();
    static void put_entry_at(char c, uint8_t color, uint16_t x, uint16_t y);
};
