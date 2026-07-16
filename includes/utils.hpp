#pragma once

#include <stdint.h>
#include <stddef.h>

void print(const char* string);
void print(uint32_t val);

void terminal_reset(void);
size_t strlen(const char* str);
uint32_t get_current_el(void);
