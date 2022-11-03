#include <memory.h>

#include "../asn1.h"
#include "asn1_inner.h"
#include "mString.h"

int ASN1_der_from_pem(uint8_t* out, size_t* outl, const char* pem) {
    char* begin = mString_strstr(pem, "-----BEGIN");
    char* end = mString_strstr(pem, "-----END");
    if (begin == NULL || end == NULL) {
        goto error;
    }
    begin = mString_next_line(begin);

    *outl = 0;
    while (begin < end) {
        size_t n;
        ASN1_assert_goto(!mString_b64_decode(out, &n, begin, '\n'), error);
        begin = mString_next_line(begin);
        out += n, *outl += n;
    }

    return 0;
error:
    return -1;
}

/// @brief get ASN1 Nodes Number from DER
/// @return SUCCESS(0),ERROR(-1)
static int _ASN1_nodes_num_from_der(size_t* n, uint8_t* in, size_t inl) {
    size_t node_num = 0;
    while (inl) {
        size_t rn;
        const ASN1_TAG* tag;
        uint32_t length;
        // read tag, length
        tag = ASN1_read_tag(&rn, in);
        in += rn, inl -= rn;
        ASN1_assert_goto(!ASN1_read_integer(&length, &rn, in), error);
        ASN1_assert_goto(inl >= rn, error);
        in += rn, inl -= rn;
        if (tag->tag_value == ASN1_TAG_SEQUENCE ||
            tag->tag_value == ASN1_TAG_SET) {
            size_t num = 0;
            ASN1_assert_goto(!_ASN1_nodes_num_from_der(&num, in, length),
                             error);
            node_num += num;
        }
        ASN1_assert_goto(inl >= length, error);
        in += length, inl -= length;
        node_num++;
    }
    *n = node_num;

    return 0;
error:
    return -1;
}

/// @brief get ASN1 Nodes from DER
/// @return SUCCESS(1),ERROR(0)
static int _ASN1_nodes_from_der(ASN1_NODE nodes[], size_t* n, uint8_t* in,
                                size_t inl) {
    size_t node_num = 0;
    ASN1_NODE _pre_node, *pre_node = &_pre_node;  // pre node
    ASN1_NODE* cur_node = nodes;                  // cur node
    ASN1_NODE* nxt_node = NULL;                   // nxt node
    while (inl) {
        size_t rn;
        cur_node->raw_data = in;
        // read tag, length
        cur_node->tag = ASN1_read_tag(&rn, in);
        in += rn, inl -= rn;
        ASN1_assert_goto(!ASN1_read_integer(&cur_node->length, &rn, in), error);
        ASN1_assert_goto(inl >= rn, error);
        in += rn, inl -= rn;
        // read node value
        cur_node->value = in;
        if (cur_node->tag->tag_value == ASN1_TAG_SEQUENCE ||
            cur_node->tag->tag_value == ASN1_TAG_SET) {
            size_t num = 0;
            cur_node->child = cur_node + 1;
            ASN1_assert_goto(!_ASN1_nodes_from_der(cur_node->child, &num, in,
                                                   cur_node->length),
                             error);
            node_num += num;
            nxt_node = cur_node + 1 + num;
        } else {
            cur_node->child = NULL;
            nxt_node = cur_node + 1;
        }
        ASN1_assert_goto(inl >= cur_node->length, error);
        in += cur_node->length, inl -= cur_node->length;
        cur_node->total_length = in - cur_node->raw_data;

        pre_node->nxt = cur_node;
        pre_node = cur_node;
        cur_node = nxt_node;
        node_num++;
    }
    cur_node->nxt = NULL;
    *n = node_num;

    return 0;
error:
    return -1;
}

int ASN1_nodes_from_der(ASN1_NODE nodes[], size_t* n, uint8_t* in, size_t inl) {
    if (nodes == NULL) {
        ASN1_assert_goto(!_ASN1_nodes_num_from_der(n, in, inl), error);
    } else {
        ASN1_assert_goto(!_ASN1_nodes_from_der(nodes, n, in, inl), error);
    }

    return 0;
error:
    return -1;
}
