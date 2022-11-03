#include "mString.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

char* mString_next_line(const char* str) {
    while (*str != 0 && *str != '\r' && *str != '\n') {
        str++;
    }
    while (*str == '\r' || *str == '\n') {
        str++;
    }
    return (char*)str;
}

char* mString_strstr(const char* str, const char* substr) {
    return strstr(str, substr);
}

char* mString_push_hex(char* hex, uint8_t* data, size_t len) {
    while (len) {
        hex += sprintf_s(hex, 4, "%02x ", *data);  // never fail
        data += 1, len -= 1;
    }
    *hex = 0;
    return hex;
}

char* mString_push_ascii(char* ascii, uint8_t* data, size_t len) {
    while (len) {
        *ascii = isprint(*data) ? (*data) : '.';
        ascii += 1, data += 1, len -= 1;
    }
    *ascii = 0;
    return ascii;
}

char* mString_push_char(char* dst, char c) {
    *dst = c;
    *(dst + 1) = 0;
    return dst + 1;
}

char* mString_push_chars(char* dst, char c, size_t len) {
    memset(dst, c, len);
    *(dst + len) = 0;
    return dst + len;
}

char* mString_push_str(char* dst, const char* s) {
    while (*s != '\0') {
        *dst = *s;
        dst += 1, s += 1;
    }
    *dst = 0;
    return dst;
}

char* mString_push_u32_hex(char* hex, uint32_t n) {
    hex += sprintf_s(hex, 12, "%d", n);  // never fail
    *hex = 0;
    return hex;
}

char* mString_b64_encode(char* b64, uint8_t* data, size_t len) {
    char* ret = b64;
    const char* base64_table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    while (len > 3) {
        uint8_t d0 = *(data + 0), d1 = *(data + 1), d2 = *(data + 2);
        *(b64 + 0) = base64_table[d0 >> 2];
        *(b64 + 1) = base64_table[(d0 & 0x3) << 4 | d1 >> 4];
        *(b64 + 2) = base64_table[(d1 & 0xf) << 2 | d2 >> 6];
        *(b64 + 3) = base64_table[d2 & 0x3f];

        b64 += 4, data += 3, len -= 3;
    }
    if (len == 2) {
        uint8_t d0 = *(data + 0), d1 = *(data + 1);
        *(b64 + 0) = base64_table[d0 >> 2];
        *(b64 + 1) = base64_table[(d0 & 0x3) << 4 | d1 >> 4];
        *(b64 + 2) = base64_table[(d1 & 0xf) << 2 | 0];
        *(b64 + 3) = '=';
        b64 += 4;
    } else if (len == 1) {
        uint8_t d0 = *(data + 0);
        *(b64 + 0) = base64_table[d0 >> 2];
        *(b64 + 1) = base64_table[(d0 & 0x3) << 4 | 0];
        *(b64 + 2) = '=';
        *(b64 + 3) = '=';
        b64 += 4;
    }
    *b64 = 0;

    return ret;
}

int mString_b64_decode(uint8_t* out, size_t* outl, const char* b64, char end) {
    static uint8_t table[256] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 253,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 62,  255, 255, 255, 63,  52,  53,  54,  55,  56,  57,  58,  59,
        60,  61,  255, 255, 255, 254, 255, 255, 255, 0,   1,   2,   3,   4,
        5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,
        19,  20,  21,  22,  23,  24,  25,  255, 255, 255, 255, 255, 255, 26,
        27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255};

    *outl = 0;
    int state = 0;
    char c[4];
    for (; *b64 != end; b64++) {
        if (*b64 == '\r' || *b64 == '\n') {
            continue;
        }
        if (state == 0 || state == 1 || state == 2) {
            c[state] = *b64;
            state++;
        } else {
            c[state] = *b64;

            uint8_t d[4] = {0};
            if (c[3] == '=' && c[2] == '=') {
                d[0] = table[(int)c[0]];
                d[1] = table[(int)c[1]];

                *(out + 0) = (d[0] << 2) | (d[1] >> 4);
                *outl += 1, out += 1;
            } else if (c[3] == '=') {
                d[0] = table[(int)c[0]];
                d[1] = table[(int)c[1]];
                d[2] = table[(int)c[2]];

                *(out + 0) = (d[0] << 2) | (d[1] >> 4);
                *(out + 1) = (d[1] << 4) | (d[2] >> 2);
                *outl += 2, out += 2;
            } else {
                d[0] = table[(int)c[0]];
                d[1] = table[(int)c[1]];
                d[2] = table[(int)c[2]];
                d[3] = table[(int)c[3]];

                *(out + 0) = (d[0] << 2) | (d[1] >> 4);
                *(out + 1) = (d[1] << 4) | (d[2] >> 2);
                *(out + 2) = (d[2] << 6) | (d[3]);
                *outl += 3, out += 3;
            }
            if (d[0] == (uint8_t)(-1) || d[1] == (uint8_t)(-1) ||
                d[2] == (uint8_t)(-1) || d[3] == (uint8_t)(-1)) {
                goto error;
            }

            state = 0;
        }
    }
    if (state != 0) {
        goto error;
    }

    return 0;
error:
    return -1;
}
