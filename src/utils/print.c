#include <stdint.h>
#include <utils.h>


volatile uint8_t* uart_ptr = (uint8_t *) 0x09000000;

void char_to_uart(const char c) {
    *uart_ptr = c;
};

void print(const char* string) {
    while (*string != '\0') {
        char_to_uart(*string);
        string++;
    };
};