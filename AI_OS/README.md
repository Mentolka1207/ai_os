# AI_OS — Руководство разработчика

## Требования

Arch Linux (VMware VM на Windows 11) с установленными:

```bash
sudo pacman -S nasm grub xorriso base-devel
```

Для кросс-компилятора x86_64-elf (bare-metal):
```bash
# Вариант 1 — через AUR
yay -S i686-elf-gcc i686-elf-binutils

# Вариант 2 — собрать вручную (GCC cross-compiler)
# https://wiki.osdev.org/GCC_Cross-Compiler
```

---

## Структура проекта

```
AI_OS/
├── boot/
│   └── boot.asm              ← Multiboot2 заголовок + точка входа (ASM)
├── kernel/
│   ├── kernel.cpp            ← kernel_main() — точка входа ядра (C++)
│   ├── linker.ld             ← Linker script (ядро по адресу 1 MiB)
│   ├── arch/x86_64/          ← (следующий шаг: IDT, GDT, paging)
│   ├── drivers/
│   │   └── terminal/
│   │       ├── terminal.h    ← VGA 80x25 драйвер
│   │       └── terminal.cpp
│   ├── mm/                   ← (следующий шаг: PMM, VMM)
│   └── proc/                 ← (следующий шаг: scheduler)
├── iso/
│   └── boot/grub/
│       └── grub.cfg          ← GRUB2 конфигурация
├── build/                    ← Артефакты сборки (git ignored)
├── Makefile
└── README.md
```

---

## Сборка

```bash
cd AI_OS
make
```

Результат: `build/aios.iso`

---

## Запуск в VMware

1. Открыть VMware → **New Virtual Machine**
2. **Custom** → Next
3. **I will install the OS later**
4. Guest OS: **Other** → **Other 64-bit**
5. RAM: минимум **128 MB**
6. Создать виртуальный диск (минимальный, 1 GB)
7. В настройках VM → **CD/DVD** → Use ISO image → выбрать `build/aios.iso`
8. Запустить VM

---

## Следующие шаги (после успешного запуска)

| Шаг | Описание | Файл |
|-----|----------|------|
| 1 | GDT (Global Descriptor Table) | `kernel/arch/x86_64/gdt.cpp` |
| 2 | IDT + ISR (прерывания) | `kernel/arch/x86_64/idt.cpp` |
| 3 | PMM (физическая память) | `kernel/mm/pmm.cpp` |
| 4 | VMM + Paging (виртуальная память) | `kernel/mm/vmm.cpp` |
| 5 | Keyboard driver (PS/2) | `kernel/drivers/keyboard/` |
| 6 | Переход в 64-bit (Long Mode) | `kernel/arch/x86_64/longmode.asm` |

---

## Полезные ссылки

- OSDev Wiki: https://wiki.osdev.org
- Multiboot2 spec: https://www.gnu.org/software/grub/manual/multiboot2/
- x86_64 ABI: https://refspecs.linuxfoundation.org/elf/x86_64-abi-0.99.pdf
