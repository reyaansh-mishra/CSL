#include <stdint.h>
#include <utils.h>

static volatile uint8_t* uart_ptr = (uint8_t *) 0x09000000;

static void dump_char_to_uart(const char c) {
    *uart_ptr = c;
};

static void string_to_uart(const char* string) {
    while (*string != '\0') {
        dump_char_to_uart(*string);
        string++;
    };
};

void print(const char* string) {
    string_to_uart(string);
};