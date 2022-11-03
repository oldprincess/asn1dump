#ifndef ASN1DUMP_ASN1_INNER_H
#define ASN1DUMP_ASN1_INNER_H

#include <stdio.h>

#include "../asn1.h"

// ====== TAG BYTES ======

#define ASN1_TAG_BOOLEAN 1
#define ASN1_TAG_INTEGER 2
#define ASN1_TAG_BIT_STRING 3
#define ASN1_TAG_OCTET_STRING 4
#define ASN1_TAG_NULL 5
#define ASN1_TAG_OBJECT_IDENTIFIER 6
#define ASN1_TAG_OBJECT_DESCRIPTOR 7
#define ASN1_TAG_EXTERNAL 8
#define ASN1_TAG_REAL 9
#define ASN1_TAG_ENUMERATED 10
#define ASN1_TAG_UTF8_STRING 12
#define ASN1_TAG_SEQUENCE 16
#define ASN1_TAG_SET 17
#define ASN1_TAG_NUMERIC_STRING 18
#define ASN1_TAG_PRINTABLE_STRING 19
#define ASN1_TAG_TELETEX_STRING 20
#define ASN1_TAG_VIDEOTEX_STRING 21
#define ASN1_TAG_IA5_STRING 22
#define ASN1_TAG_UTC_TIME 23
#define ASN1_TAG_GENERALIZED_TIME 24
#define ASN1_TAG_GRAPHIC_STRING 25
#define ASN1_TAG_VISIBLE_STRING 26
#define ASN1_TAG_GENERAL_STRING 27
#define ASN1_TAG_UNIVERSAL_STRING 28
#define ASN1_TAG_BMP_STRING 30

#define ASN1_assert_goto(exp, tag) \
    do {                           \
        if (!(exp)) {              \
            goto tag;              \
        }                          \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

/// @brief ASN1 read TAG from DER
/// @param[out] rn  number of read bytes(always 1)
/// @param[in] in   input buffer(DER)
/// @return ASN1_TAG
const ASN1_TAG* ASN1_read_tag(size_t* rn, uint8_t* in);

/// @brief ASN1 read Integer from DER
/// @param[out] integer Integer
/// @param[out] rn      number of read bytes(always 1)
/// @param[in] in       input buffer(DER)
/// @return SUCCESS(0),ERROR(-1)
int ASN1_read_integer(uint32_t* integer, size_t* rn, uint8_t* in);

#endif  // ASN1DUMP_ASN1_INNER_H
