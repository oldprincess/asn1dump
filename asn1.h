#ifndef ASN1DUMP_ASN1_H
#define ASN1DUMP_ASN1_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ASN1_TAG {
    const char* name;
    uint8_t tag_value;
} ASN1_TAG;

typedef struct ASN1_NODE {
    // <tag> <length> <value>
    const ASN1_TAG* tag;
    uint32_t length;
    uint8_t* value;

    uint8_t* raw_data;
    size_t total_length;

    struct ASN1_NODE* nxt;    // next Node(in the same depth)
    struct ASN1_NODE* child;  // for SEQUENCE/SET
} ASN1_NODE;

/// @brief Decode PEM to DER
/// @param[out] out     output buffer(DER)
/// @param[out] outl    output length
/// @param[in] pem      input buffer(PEM)
/// @return SUCCESS(0),ERROR(-1)
int ASN1_der_from_pem(uint8_t* out, size_t* outl, const char* pem);

/// @brief Read ASN1 Nodes from DER
/// @param[out] nodes   Nodes Buffer(optional)
/// @param[out] n       Nodes Num
/// @param[in] in       input buffer(DER)
/// @param[in] inl      input length
int ASN1_nodes_from_der(ASN1_NODE nodes[], size_t* n, uint8_t* in, size_t inl);

/// @brief Dump ASN1 single Node to String
/// @param[in] s        String buffer
/// @param[in] node     ASN1 Node ptr
/// @return s(input)
char* ASN1_node_str(char* s, ASN1_NODE* node);

/// @brief Dump ASN1 Nodes in same depth to String
/// @param[in] s        String buffer
/// @param[in] nodes    ASN1 Nodes Buffer
/// @return s(input)
char* ASN1_nodes_str(char* s, ASN1_NODE nodes[]);

#ifdef __cplusplus
}
#endif

#endif  // ASN1DUMP_ASN1_H
