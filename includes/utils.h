#pragma once
#include <stdint.h>

volatile uint8_t* uart_ptr = (uint8_t *) 0x09000000;

void print(const char* string);