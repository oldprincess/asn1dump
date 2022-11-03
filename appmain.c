#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "asn1.h"

#define ASN1DUMP_FROM_PEM 1
#define ASN1DUMP_FROM_DER 2

#define ASN1DUMP_HELP "asn1dump -in <input file> -from <PEM|DER>"

static const char* filename = NULL;
static int from = -1;

#define BUF_SIZE (1024 * 1024)

static char pem_data[BUF_SIZE] = {0};
static uint8_t der_data[BUF_SIZE] = {0};
static size_t der_size = 0;
static char sbuf[10 * BUF_SIZE] = {0};
static ASN1_NODE nodes[100];

static void argparse(int argc, char* argv[]) {
    // fetch params
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-help") == 0) {
            puts(ASN1DUMP_HELP);
            exit(0);
        }

        if (strcmp(argv[i], "-in") == 0 && i + 1 < argc) {
            filename = argv[i + 1];
        }
        if (strcmp(argv[i], "-from") == 0 && i + 1 < argc) {
            if (strcmp(argv[i + 1], "DER") == 0) {
                from = ASN1DUMP_FROM_DER;
            }
            if (strcmp(argv[i + 1], "PEM") == 0) {
                from = ASN1DUMP_FROM_PEM;
            }
        }
    }
    if (filename == NULL || from == -1) {
        puts("Invalid params, see: " ASN1DUMP_HELP);
        exit(-1);
    }
}

static size_t fileread(uint8_t* buf, const char* filename) {
    struct stat statbuf;
    FILE* fp = NULL;
    size_t fsize;
    if (stat(filename, &statbuf)) {
        printf("Error in open file `%s`\n", filename);
        exit(-1);
    }
    fsize = statbuf.st_size;
    if (!(fp = fopen(filename, "rb"))) {
        printf("Error in open file `%s`\n", filename);
        exit(-1);
    }
    if (fsize >= BUF_SIZE) {
        puts("filesize is too large");
        exit(-1);
    }
    memset(buf, 0, fsize);
    fread(buf, 1, fsize, fp);
    fclose(fp);
    return fsize;
}

int appmain(int argc, char* argv[]) {
    argparse(argc, argv);

    // alloc der data
    if (from == ASN1DUMP_FROM_DER) {
        der_size = fileread(der_data, filename);
    } else {
        size_t pem_len = fileread((uint8_t*)pem_data, filename);
        pem_data[pem_len] = 0;  // end of string
        if (ASN1_der_from_pem(der_data, &der_size, pem_data)) {
            puts("Invalid PEM data");
            exit(-1);
        }
    }
    //
    size_t node_num = 0;
    if (ASN1_nodes_from_der(NULL, &node_num, der_data, der_size)) {
        puts("Invalid DER data");
        exit(-1);
    }
    if (node_num > sizeof(nodes) / sizeof(ASN1_NODE)) {
        puts("too much ASN1 node num");
        exit(-1);
    }
    if (ASN1_nodes_from_der(nodes, &node_num, der_data, der_size)) {
        puts("Invalid DER data");
        exit(-1);
    }
    printf("%s\n", ASN1_nodes_str(sbuf, nodes));

    return 0;
}
