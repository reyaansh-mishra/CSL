#pragma once
#include <csl.h>

void print(const char* string);
void print(uint64_t val);
void print(int val);
void print(bool state);

void pr_info(const char* pre_messgae, const char* string);
void pr_info(const char* pre_message, const bool state);

void terminal_reset(void);
