#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "asn1.h"

static ASN1_NODE nodes[20];
static ASN1_NODE nodes2[20];
static char sbuf[1024 * 1024];
static char pem_buf[1024 * 1024];
static uint8_t der_data[1024 * 1024];
static size_t der_size;

// INTEGER
// SEQUENCE
//    OBJECT_IDENTIFIER
//    NULL
// OCTET_STRING
static const char* filename = "./test/rsa_skey.pem";

#define PRINT_ERROR() printf("Error in " __FILE__ " ,line %d\n", __LINE__)

void demo(void) {
    // load file data
    FILE* fp = NULL;
    struct stat statbuf;
    if (stat(filename, &statbuf)) {
        PRINT_ERROR();
        exit(-1);
    }
    memset(pem_buf, 0, statbuf.st_size + 1);
    if (fopen_s(&fp, filename, "rb")) {
        PRINT_ERROR();
        exit(-1);
    }
    fread(pem_buf, 1, statbuf.st_size, fp);
    fclose(fp);
    // der from pem
    if (ASN1_der_from_pem(der_data, &der_size, pem_buf)) {
        PRINT_ERROR();
        exit(-1);
    }
    // nodes from der
    size_t n;
    if (ASN1_nodes_from_der(nodes, &n, der_data, der_size)) {
        PRINT_ERROR();
        exit(-1);
    }
    ASN1_NODE* node = nodes[0].child->nxt->nxt;
    if (ASN1_nodes_from_der(nodes2, &n, node->value, node->length)) {
        PRINT_ERROR();
        exit(-1);
    }
    printf("%s\n", ASN1_nodes_str(sbuf, nodes2));
}
