#include "asn1_inner.h"
#include "mString.h"

// =============================================
// ========== ASN1 NODES to Str ================
// =============================================

#define ASN1_DUMPSIZE 10

#define STR_VALUE_PREFIX "|  "
#define STR_COMMENT_PREFIX " ; "
#define STR_PLACEHOLDER "   "
#define CHAR_SPACEBAR ' '
#define CHAR_NEWLINE '\n'

/// @brief ASN1 <Value> to Str
/// @param[in] s        string buffer
/// @param[in] depth    depth of nodes
/// @param[in] mem      memmory buffer
/// @param[in] len      length of mem
/// @return current string buffer ptr
static char* _ASN1_dump_str(char* s, size_t depth, uint8_t* mem, size_t len) {
    // |  xx xx .. xx ; xxxx..
    while (len >= ASN1_DUMPSIZE) {
        s = mString_push_chars(s, CHAR_SPACEBAR, 3 * depth - 3);
        s = mString_push_str(s, STR_VALUE_PREFIX);
        s = mString_push_hex(s, mem, ASN1_DUMPSIZE);
        s = mString_push_str(s, STR_COMMENT_PREFIX);
        s = mString_push_str(s, STR_PLACEHOLDER);
        s = mString_push_ascii(s, mem, ASN1_DUMPSIZE);
        s = mString_push_char(s, CHAR_NEWLINE);
        mem += ASN1_DUMPSIZE, len -= ASN1_DUMPSIZE;
    }
    if (len) {
        s = mString_push_chars(s, CHAR_SPACEBAR, 3 * depth - 3);
        s = mString_push_str(s, STR_VALUE_PREFIX);
        s = mString_push_hex(s, mem, len);
        s = mString_push_chars(s, CHAR_SPACEBAR, (ASN1_DUMPSIZE - len) * 3);
        s = mString_push_str(s, STR_COMMENT_PREFIX);
        s = mString_push_str(s, STR_PLACEHOLDER);
        s = mString_push_ascii(s, mem, len);
        s = mString_push_char(s, CHAR_NEWLINE);
    }
    return s;
}

/// @brief ASN1 <Value> to Str
/// @param[in] s        string buffer
/// @param[in] depth    depth of nodes
/// @param[in] mem      memmory buffer
/// @param[in] len      length of mem
/// @return current string buffer ptr
static char* _ASN1_dump_mem(char* s, size_t depth, uint8_t* mem, size_t len) {
    // |  xx xx .. xx
    while (len) {
        size_t size = (len >= ASN1_DUMPSIZE) ? ASN1_DUMPSIZE : len;
        s = mString_push_chars(s, CHAR_SPACEBAR, 3 * depth - 3);
        s = mString_push_str(s, STR_VALUE_PREFIX);
        s = mString_push_hex(s, mem, size);
        s = mString_push_char(s, CHAR_NEWLINE);
        mem += size, len -= size;
    }
    return s;
}

/// @brief ASN1 <Node> to Str
/// @param[in] s        string buffer
/// @param[in] depth    depth of nodes
/// @param[in] node     ASN1 node
/// @return current string buffer ptr
static char* _ASN1_str(char* s, size_t depth, ASN1_NODE* node) {
    size_t n = node->value - node->raw_data;
    // dump header
    // xx xx xx ; <tag>(<length> Bytes)
    s = mString_push_chars(s, CHAR_SPACEBAR, 3 * depth);
    s = mString_push_hex(s, node->raw_data, n);
    s = mString_push_chars(s, CHAR_SPACEBAR, 3 * ASN1_DUMPSIZE + 3 - 3 * n);
    s = mString_push_str(s, STR_COMMENT_PREFIX);
    s = mString_push_str(s, node->tag->name);
    s = mString_push_str(s, " (");
    s = mString_push_u32_hex(s, node->length);
    s = mString_push_str(s, " Bytes)\n");
    // dump value
    depth += 1;
    switch (node->tag->tag_value) {
        case ASN1_TAG_NULL:
            break;  // do nothing
        case ASN1_TAG_SEQUENCE:
        case ASN1_TAG_SET:
            for (ASN1_NODE* p = node->child; p != NULL; p = p->nxt) {
                s = _ASN1_str(s, depth, p);
            }
            break;
        case ASN1_TAG_BIT_STRING:
        case ASN1_TAG_BOOLEAN:
        case ASN1_TAG_INTEGER:
            s = _ASN1_dump_mem(s, depth, node->value, node->length);
            break;
        default:
            s = _ASN1_dump_str(s, depth, node->value, node->length);
    }

    return s;
}

char* ASN1_node_str(char* s, ASN1_NODE* node) {
    char* ret = s;
    s = _ASN1_str(s, 0, node);
    *s = 0;
    return ret;
}

char* ASN1_nodes_str(char* s, ASN1_NODE nodes[]) {
    char* ret = s;
    for (ASN1_NODE* p = nodes; p != NULL; p = p->nxt) {
        s = _ASN1_str(s, 0, p);
    }
    *s = 0;
    return ret;
}
