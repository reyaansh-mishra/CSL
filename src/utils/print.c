#include "ProcessorBind.h"
#include <stdint.h>
#include <utils.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <csl.h>

#define CSL_PRINT_BUFFER_SIZE 512


static volatile uint8_t* uart_ptr = (uint8_t *) 0x09000000;

/* -------------------------------------------------------- */
/* General Util funcs to be realloced later */
/* -------------------------------------------------------- */

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

/* -------------------------------------------------------- */
/* FB + UART Logic using EFI */
/* -------------------------------------------------------- */

void efi_print(const char *str) {

	CHAR16 buf_out[CSL_PRINT_BUFFER_SIZE];
	size_t i_in = 0;
	size_t i_out = 0;
	size_t string_length = strlen(str);

	for (; i_in < string_length; i_in++) {	/* CURRENTLY WE USED GO OOB */
		if (str[i_in] == '\n') {
			buf_out[i_out] = L'\r';
			buf_out[i_out+1] = L'\n';
			i_out++;
		} else {
			buf_out[i_out] = str[i_in];
		};

		i_out++;
	};
	buf_out[i_out] = L'\0';

	efi_context.SystemTable->ConOut->OutputString(efi_context.SystemTable->ConOut, buf_out);
};

void print(const char* string) {
    efi_print(string);
};