/*
 * MIT License
 *
 * Copyright (c) 2022 IridiumProject
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <common/string.h>
#include <common/log.h>

size_t strlen(const char* str)
{
    size_t i = 0;
    while (str[i++]);
    return i - 1;
}


bool memcmp(const char* str1, const char* str2, size_t n)
{
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return false;
        }

        ++str1;
        ++str2;
    }

    return true;
}

char* dec2str(int number) {
    static uint8_t dec_string[80];
    uint8_t i = 0, j, temp;
    uint8_t negative = 0;       // Is number negative?

    if (number == 0) dec_string[i++] = '0'; // If passed in 0, print a 0
    else if (number < 0)  {
        negative = 1;       // Number is negative
        number = -number;   // Easier to work with positive values
    }

    while (number > 0) {
        dec_string[i] = (number % 10) + '0';
        number /= 10;
        i++;
    }

    if (negative) dec_string[i++] = '-';

    dec_string[i] = '\0';

    i--;
    for (j = 0; j < i; j++, i--) {
        temp          = dec_string[j];
        dec_string[j] = dec_string[i];
        dec_string[i] = temp;
    }

    return dec_string;
}






uint8_t* hex2str(uint32_t hex_num) {
    static uint8_t hex_string[80];
    uint8_t *ascii_numbers = "0123456789ABCDEF";
    uint8_t nibble;
    uint8_t i = 0, j, temp;
    uint8_t pad = 0;
    
    // If passed in 0, print a 0
    if (hex_num == 0) {
        strncpy(hex_string, "0\0", 2);
        i = 1;
    }

    if (hex_num < 0x10) pad = 1;    // If one digit, will pad out to 2 later

    while (hex_num > 0) {
        // Convert hex values to ascii string
        nibble = (uint8_t)hex_num & 0x0F;  // Get lowest 4 bits
        nibble = ascii_numbers[nibble];    // Hex to ascii 
        hex_string[i] = nibble;             // Move ascii char into string
        hex_num >>= 4;                     // Shift right by 4 for next nibble
        i++;
    }
        
    if (pad) hex_string[i++] = '0';  // Pad out string with extra 0    

    // Add initial "0x" to front of hex string
    hex_string[i++] = 'x';
    hex_string[i++] = '0';
    hex_string[i] = '\0';   // Null terminate string

    // Number is stored backwards in hex_string, reverse the string by swapping ends
    //   until they meet in the middle
    i--;    // Skip null byte
    for (j = 0; j < i; j++, i--) {
        temp          = hex_string[j];
        hex_string[j] = hex_string[i];
        hex_string[i] = temp;
    }

    // Print hex string
    return hex_string;
}

uint8_t* strncpy(uint8_t *dst, const uint8_t *src, const uint8_t len) {
    for (uint8_t i = 0; src[i] && i < len; i++)
        dst[i] = src[i];

    return dst;
}


void memzero(void* ptr, size_t n)
{
	char* ptr_ch = ptr;

	for (size_t i = 0; i < n; ++i)
	{
        // kprintf("A, %d\n", i);
		ptr_ch[i] = 0;
	}
}


void memset(void* ptr, uint64_t data, size_t n) {
    char* ptr_ch = ptr;

	for (size_t i = 0; i < n; ++i)
	{
		ptr_ch[i] = data;
	}
}


uint8_t strcmp(const char* str1, const char* str2)
{
    uint32_t str1_len, str2_len;
    str1_len = strlen(str1);
    str2_len = strlen(str2);

    if (str1_len != str2_len)
    {
        return 1;
    }

    for (uint32_t i = 0; i < str1_len; ++i)
    {
        if (str1[i] != str2[i])
        {
            return 1;
        }
    }

    return 0;
}


uint8_t strncmp(const char* str1, const char* str2, size_t n)
{
    for (uint32_t i = 0; i < n; ++i)
    {
        if (str1[i] != str2[i])
        {
            return 1;
        }
    }

    return 0;
}
