#include "asn1_inner.h"

// =======================================
// =========== ASN1 TAG DEF ==============
// =======================================

// ===== DEFINE TAG ITEM ====

#define ASN1_TAG_DEFINE(tag)              \
    static const ASN1_TAG _ASN1_##tag = { \
        .name = #tag,                     \
        .tag_value = ASN1_TAG_##tag,      \
    }

ASN1_TAG_DEFINE(BOOLEAN);
ASN1_TAG_DEFINE(INTEGER);
ASN1_TAG_DEFINE(BIT_STRING);
ASN1_TAG_DEFINE(OCTET_STRING);
ASN1_TAG_DEFINE(NULL);
ASN1_TAG_DEFINE(OBJECT_IDENTIFIER);
ASN1_TAG_DEFINE(OBJECT_DESCRIPTOR);
ASN1_TAG_DEFINE(EXTERNAL);
ASN1_TAG_DEFINE(REAL);
ASN1_TAG_DEFINE(ENUMERATED);
ASN1_TAG_DEFINE(UTF8_STRING);
ASN1_TAG_DEFINE(SEQUENCE);
ASN1_TAG_DEFINE(SET);
ASN1_TAG_DEFINE(NUMERIC_STRING);
ASN1_TAG_DEFINE(PRINTABLE_STRING);
ASN1_TAG_DEFINE(TELETEX_STRING);
ASN1_TAG_DEFINE(VIDEOTEX_STRING);
ASN1_TAG_DEFINE(IA5_STRING);
ASN1_TAG_DEFINE(UTC_TIME);
ASN1_TAG_DEFINE(GENERALIZED_TIME);
ASN1_TAG_DEFINE(GRAPHIC_STRING);
ASN1_TAG_DEFINE(VISIBLE_STRING);
ASN1_TAG_DEFINE(GENERAL_STRING);
ASN1_TAG_DEFINE(UNIVERSAL_STRING);
ASN1_TAG_DEFINE(BMP_STRING);

static const ASN1_TAG* _tagList[32] = {
    NULL,
    &_ASN1_BOOLEAN,
    &_ASN1_INTEGER,
    &_ASN1_BIT_STRING,
    &_ASN1_OCTET_STRING,
    &_ASN1_NULL,
    &_ASN1_OBJECT_IDENTIFIER,
    &_ASN1_OBJECT_DESCRIPTOR,
    &_ASN1_EXTERNAL,
    &_ASN1_REAL,
    &_ASN1_ENUMERATED,
    NULL,
    &_ASN1_UTF8_STRING,
    NULL,
    NULL,
    NULL,
    &_ASN1_SEQUENCE,
    &_ASN1_SET,
    &_ASN1_NUMERIC_STRING,
    &_ASN1_PRINTABLE_STRING,
    &_ASN1_TELETEX_STRING,
    &_ASN1_VIDEOTEX_STRING,
    &_ASN1_IA5_STRING,
    &_ASN1_UTC_TIME,
    &_ASN1_GENERALIZED_TIME,
    &_ASN1_GRAPHIC_STRING,
    &_ASN1_VISIBLE_STRING,
    &_ASN1_GENERAL_STRING,
    &_ASN1_UNIVERSAL_STRING,
    NULL,
    &_ASN1_BMP_STRING,
    NULL,
};

// =======================================
// =========== ASN1 TAG FUNC =============
// =======================================

const ASN1_TAG* ASN1_read_tag(size_t* rn, uint8_t* in) {
    static const ASN1_TAG _ASN1_TAG_UNKOWN = {.name = "UNKNOWN"};
    *rn = 1;
    uint8_t tag_value = *in & 0b11111;
    return _tagList[tag_value] == NULL ? &_ASN1_TAG_UNKOWN
                                       : _tagList[tag_value];
}

int ASN1_read_integer(uint32_t* integer, size_t* rn, uint8_t* in) {
    *rn = (*in <= 0x7f) ? 1 : 1 + (*in & 0x7f);
    if (*in <= 0x7f) {
        *integer = *in;
    } else {
        *integer = 0;
        int num = *in & 0x7f;
        if (num > sizeof(uint32_t)) {
            goto error;
        }
        for (int i = 0; i < num; i++) {
            *integer = (*integer << 8) | *(in + 1 + i);
        }
    }

    return 0;
error:
    return -1;
}
