/* src/terminal/terminal.cpp */

#include <utils.hpp>
#include <stdarg.h>


#define UART0_BASE              0x09000000

volatile uint32_t* const uart = (volatile uint32_t*)UART0_BASE;


void terminal_reset() {
    efi.SystemTable->ConOut->ClearScreen(efi.SystemTable->ConOut);
};

/* -------------------------------------------------------- */
/* General Util func(s) to be relocated later */
/* -------------------------------------------------------- */

inline size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

/* -------------------------------------------------------- */
/* FB + UART Logic */
/* -------------------------------------------------------- */

static void put_char(const char c) {
    if (c == '\n')
        *uart = '\r';

    *uart = c;
};

void dump_str_to_uart(const char* str, size_t string_length) {
    for (size_t i = 0; i < string_length; i++) {
        put_char(str[i]);
    };
};

static inline void puts_raw(const char *s)  /* NOPE. dump_str_to_uart is canotical or whatever the spelling is. */
{
    dump_str_to_uart(s, strlen(s));
};

void efi_print(const char *str) {
    size_t string_len = strlen(str);

    dump_str_to_uart(str, string_len);
};

void vprint(const char* fmt, va_list args)
{
    while (*fmt) {
        if (*fmt != '%') {
            put_char(*fmt++);
            continue;
        }

        fmt++; // Skip '%'

        switch (*fmt++) {
            case '%':
                put_char('%');
                break;

            case 'c': {
                char c = (char)va_arg(args, int);
                put_char(c);
                break;
            }

            case 's': {
                const char* s = va_arg(args, const char*);
                if (!s)
                    s = "(null)";
                puts_raw(s);
                break;
            }

            case 'd': {
                int n = va_arg(args, int);
                print(n);      // TODO
                break;
            }

            case 'u': {
                unsigned n = va_arg(args, unsigned);
                print((uint64_t)n);     // TODO
                break;
            }

            case 'x': {
                unsigned n = va_arg(args, unsigned);
                print((uint64_t)n);      // TODO
                break;
            }

            case 'p': {
                uintptr_t p = va_arg(args, uintptr_t);
                efi_print("0x");
                print(p);      // TODO
                break;
            }

            default:
                put_char('%');
                put_char(fmt[-1]); // Unknown specifier
                break;
        }
    };
};

void print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint(fmt, args);
    va_end(args);
}

void print(uint64_t val) {
    char buf[24]; // enough for a 64-bit uint + null terminator
    int i = 22;
    buf[23] = '\0';

    if (val == 0) {
        put_char('0');
        return;
    }

    while (val > 0 && i >= 0) {
        buf[i] = '0' + (val % 10);
        val /= 10;
        i--;
    }

    puts_raw(&buf[i + 1]);
};

void print(int val)
{
    if (val < 0) {
        put_char('-');
        print((uint64_t)(-(int64_t)val));
        return;
    }

    print((uint64_t)val);
};

void print(bool state) {
    if (state) puts_raw("TRUE");
    else puts_raw("FALSE");
};

void pr_info(const char* pre_message, const char* string) {
    print(pre_message);
    print(string);
};

void pr_info(const char* pre_message, const bool state) {
    print(pre_message);
    print(state);
};

void pr_newline() { print("\n"); };

void print_hex(const uint64_t val) {
    puts_raw("0x");
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        char c = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
        put_char(c);
    };
};


/* DUMP YAY */

extern "C" void exception_dump(uint64_t esr, uint64_t far, uint64_t elr, uint64_t spsr, uint64_t exception) {
    INFO("Taking exception\n");
    print("ESR: "); print_hex(esr); pr_newline();
    print("FAR: "); print_hex(far); pr_newline();
    print("ELR: "); print_hex(elr); pr_newline();
    print("SPSR: "); print_hex(spsr); pr_newline();
    print("EXCEPTION: ");print(exception);pr_newline();
    // deliberately no eret — halt below in vec_common instead
};
