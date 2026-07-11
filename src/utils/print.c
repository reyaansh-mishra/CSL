#include <stdint.h>
#include <utils.h>

void char_to_uart(const char c) {
    *uart_ptr = c;
};

void print(const char* string) {
    while (*string != '\0') {
        char_to_uart(*string);
        string++;
    };
};