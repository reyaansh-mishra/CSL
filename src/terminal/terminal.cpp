/* src/terminal/terminal.cpp */

#include <utils.hpp>

#define CSL_PRINT_BUFFER_SIZE 512

void terminal_reset() {
    efi.SystemTable->ConOut->ClearScreen(efi.SystemTable->ConOut);
};

/* -------------------------------------------------------- */
/* General Util func(s) to be relocated later */
/* -------------------------------------------------------- */

extern "C" inline size_t strlen(const char* str) 
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
		efi.SystemTable->ConOut->OutputString(efi.SystemTable->ConOut, (CHAR16*)u"ERR_PRINT_MESSAGE_LOST. Message Too Big Not printing message.");
		return;
	};

    for (; i_in < string_length && i_out < CSL_PRINT_BUFFER_SIZE - 1; i_in++) {
        if (str[i_in] == '\n') {
            if (i_out >= CSL_PRINT_BUFFER_SIZE - 2) {	// need room for \r\n + terminator
				efi.SystemTable->ConOut->OutputString(efi.SystemTable->ConOut, (CHAR16*)u"ERR_PRINT_MESSAGE_LOST. BUFFER ALMOST DIED. Not printing message.");
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

	efi.SystemTable->ConOut->OutputString(efi.SystemTable->ConOut, buf_out);
};

void print(const char* string) {
    efi_print(string);
};

void print(uint64_t val) {
    char buf[24]; // enough for a 64-bit uint + null terminator
    int i = 22;
    buf[23] = '\0';

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
};

void print(int val) {
    if (val < 0) {
        print("-");
        print((uint64_t)(-(int64_t)val));  // careful with INT_MIN edge case
        return;
    }
    print((uint64_t)val);
};

void print(bool state) {
    if (state) print("TRUE");
    else print("FALSE");
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
