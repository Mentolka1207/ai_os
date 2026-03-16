// =============================================================================
// AI_OS — VGA Text Mode Terminal Driver
// File:  kernel/drivers/terminal/terminal.cpp
// =============================================================================

#include "terminal.h"

// --- Статические члены класса ---
uint16_t  Terminal::row_    = 0;
uint16_t  Terminal::col_    = 0;
uint8_t   Terminal::color_  = 0;
uint16_t* Terminal::buffer_ = nullptr;

// =============================================================================
void Terminal::init() {
    row_    = 0;
    col_    = 0;
    color_  = vga_entry_color(VGAColor::LightGreen, VGAColor::Black);
    buffer_ = reinterpret_cast<uint16_t*>(VGA_MEMORY);
    clear();
}

// =============================================================================
void Terminal::set_color(VGAColor fg, VGAColor bg) {
    color_ = vga_entry_color(fg, bg);
}

// =============================================================================
void Terminal::clear() {
    const uint16_t blank = vga_entry(' ', color_);
    for (uint16_t y = 0; y < VGA_HEIGHT; y++)
        for (uint16_t x = 0; x < VGA_WIDTH; x++)
            buffer_[y * VGA_WIDTH + x] = blank;
    row_ = 0;
    col_ = 0;
}

// =============================================================================
void Terminal::put_entry_at(char c, uint8_t color, uint16_t x, uint16_t y) {
    buffer_[y * VGA_WIDTH + x] = vga_entry(c, color);
}

// =============================================================================
void Terminal::scroll() {
    // Сдвигаем все строки на одну вверх
    for (uint16_t y = 1; y < VGA_HEIGHT; y++)
        for (uint16_t x = 0; x < VGA_WIDTH; x++)
            buffer_[(y - 1) * VGA_WIDTH + x] = buffer_[y * VGA_WIDTH + x];

    // Очистить последнюю строку
    const uint16_t blank = vga_entry(' ', color_);
    for (uint16_t x = 0; x < VGA_WIDTH; x++)
        buffer_[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;

    row_ = VGA_HEIGHT - 1;
}

// =============================================================================
void Terminal::put_char(char c) {
    if (c == '\n') {
        col_ = 0;
        if (++row_ >= VGA_HEIGHT)
            scroll();
        return;
    }

    if (c == '\r') {
        col_ = 0;
        return;
    }

    if (c == '\t') {
        // Tab = 4 пробела
        for (int i = 0; i < 4; i++) put_char(' ');
        return;
    }

    put_entry_at(c, color_, col_, row_);

    if (++col_ >= VGA_WIDTH) {
        col_ = 0;
        if (++row_ >= VGA_HEIGHT)
            scroll();
    }
}

// =============================================================================
void Terminal::write(const char* str) {
    while (*str)
        put_char(*str++);
}

// =============================================================================
void Terminal::writeln(const char* str) {
    write(str);
    put_char('\n');
}

// =============================================================================
void Terminal::write_hex(uint32_t value) {
    static const char hex_chars[] = "0123456789ABCDEF";
    write("0x");
    for (int i = 28; i >= 0; i -= 4)
        put_char(hex_chars[(value >> i) & 0xF]);
}

// =============================================================================
void Terminal::write_dec(uint32_t value) {
    if (value == 0) { put_char('0'); return; }

    char buf[12];
    int  len = 0;

    while (value > 0) {
        buf[len++] = '0' + (value % 10);
        value /= 10;
    }

    // Вывести в обратном порядке
    for (int i = len - 1; i >= 0; i--)
        put_char(buf[i]);
}

// =============================================================================
void Terminal::set_cursor(uint16_t col, uint16_t row) {
    col_ = col < VGA_WIDTH  ? col : VGA_WIDTH  - 1;
    row_ = row < VGA_HEIGHT ? row : VGA_HEIGHT - 1;
}
