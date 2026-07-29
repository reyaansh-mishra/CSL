/* includes/CSL/terminal.hpp */

#pragma once
#include <csl.h>

void print(const char* string, ...);
void print(uint64_t val);
void print_hex(uint64_t val);

void print(int val);
void print(bool state);

void pr_newline();

void terminal_reset(void);
