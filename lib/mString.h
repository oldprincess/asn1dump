#ifndef ASN1DUMP_MSTRING_H
#define ASN1DUMP_MSTRING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief search substr in str, equal to strstr
/// @param[in] str
/// @param[in] substr
/// @return ptr of substr
char* mString_strstr(const char* str, const char* substr);

/// @brief pass this line
/// @param[in] s    input str
/// @return next line ptr
char* mString_next_line(const char* s);

/// @brief Decode from Base64
/// @param[out] out     output buffer
/// @param[out] outl    output len
/// @param[in] b64      base64 string
/// @param[in] end      end char(for example, '\0')
/// @return SUCCESS(0),ERROR(-1)
int mString_b64_decode(uint8_t* out, size_t* outl, const char* b64, char end);

/// @brief Encode to Base64
/// @param[out] b64     Base64 string output
/// @param[in] data     input data
/// @param[in] len      input length
/// @return b64 ptr
char* mString_b64_encode(char* b64, uint8_t* data, size_t len);

// =======================================================
// ============ see string as a stack ====================
// =======================================================

/// @brief Cast data to Hex, add to the end of string
/// @param[out] hex     output hex
/// @param[in] data     input data
/// @param[in] len      data length
/// @return end of hex, ptr
char* mString_push_hex(char* hex, uint8_t* data, size_t len);

/// @brief Cast data to ascii, add to the end of string
/// @param[out] ascii   output ascii
/// @param[in] data     input data
/// @param[in] len      data length
/// @return end of ascii, ptr
char* mString_push_ascii(char* ascii, uint8_t* data, size_t len);

/// @brief add char to end of string
/// @param[out] dst     output
/// @param[in] c        input char
/// @return end of string, ptr
char* mString_push_char(char* dst, char c);

/// @brief add chars to end of string
/// @param[out] dst     output
/// @param[in] c        input char
/// @param[in] len      num of char
/// @return end of string, ptr
char* mString_push_chars(char* dst, char c, size_t len);

/// @brief add string to end of string
/// @param[out] dst     output
/// @param[in] s        input string
/// @return end of string, ptr
char* mString_push_str(char* dst, const char* s);

/// @brief add 32bit unsigned integer to end of string
/// @param[out] dst     output
/// @param[in] n        32bit unsigned integer
/// @return end of string, ptr
char* mString_push_u32_hex(char* hex, uint32_t n);

#ifdef __cplusplus
}
#endif

#endif  // ASN1DUMP_MSTRING_H
