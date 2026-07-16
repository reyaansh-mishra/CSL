#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <csl.h>
#include <utils.hpp>

#define CSL_PRINT_BUFFER_SIZE 512

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

void efi_print(const char *str) {		/* MAX SUPPORTED CSL_PRINT_BUFFER_SIZE LENGTH*/

	CHAR16 buf_out[CSL_PRINT_BUFFER_SIZE];
	size_t i_in = 0;
	size_t i_out = 0;
	size_t string_length = strlen(str);

	if (string_length >= CSL_PRINT_BUFFER_SIZE) {
		efi_context.SystemTable->ConOut->OutputString(efi_context.SystemTable->ConOut, (CHAR16*)u"ERR_PRINT_MESSAGE_LOST. Message Too Big Not printing message.");
		return;
	};

    for (; i_in < string_length && i_out < CSL_PRINT_BUFFER_SIZE - 1; i_in++) {
        if (str[i_in] == '\n') {
            if (i_out >= CSL_PRINT_BUFFER_SIZE - 2) {	// need room for \r\n + terminator
				efi_context.SystemTable->ConOut->OutputString(efi_context.SystemTable->ConOut, (CHAR16*)u"ERR_PRINT_MESSAGE_LOST. BUFFER ALMOST DIED. Not printing message.");
				return;
			};

            buf_out[i_out] = L'\r';
            buf_out[i_out + 1] = L'\n';
            i_out += 2;
        } else {
            buf_out[i_out] = str[i_in];
            i_out++;
        };
	};

	buf_out[i_out] = L'\0';

	efi_context.SystemTable->ConOut->OutputString(efi_context.SystemTable->ConOut, buf_out);
};

void print(const char* string) {
    efi_print(string);
};

void print(uint32_t val) {
    char buf[12]; // enough for a 32-bit uint + null terminator
    int i = 10;
    buf[11] = '\0';

    if (val == 0) {
        print("0");
        return;
    }

    while (val > 0 && i >= 0) {
        buf[i] = '0' + (val % 10);
        val /= 10;
        i--;
    }

    print(&buf[i + 1]);
}