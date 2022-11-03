# ASN1DUMP

**NOTE**: I use Baidu Translator to translate this file

[中文 README 文件](README.md)

The asn1 printing tool supports parsing data in PEM or DER format and outputting it in ASN1 format. For the basic knowledge of ASN1, refer to Microsoft's [Related Documents](https://learn.microsoft.com/en-us/windows/win32/seccertenroll/distinguished-encoding-rules)

## 1. Quick start

```text
│ asn1.h        external interface header file
│ appmain.c     APP application (console)
│ demo.c        Sample code
│ main.c        Main function
│
└─lib           code directory
    asn1_der.c      ASN1 and DER/PEM conversion
    asn1_dump.c     Print ASN1 data
    asn1_inner.c    ASN1 function
    asn1_inner.h    ASN1 function
    mString.c       String processing function
    mString.h       String processing function
```

Compilation Example

```text
gcc -o asn1dump.exe *.c lib/*.c
```

Command line parameters

```text
asn1dump -in <input file> -from <PEM|DER>
```

## 2. Operation test

**Note**: The files used in this section have been placed in the *test* directory.

Use OPENSSL to generate PEM and DER format files

```text
#Generate 1024 bit RSA key in PEM format
$ openssl genrsa -out rsa_skey.pem 1024

#Convert the key to traditional format
$ openssl rsa -in rsa_skey.pem -out rsa_skey_t.pem -outform pem -traditional

#Convert PEM format key to DER format
$ openssl rsa -in rsa_skey.pem -out rsa_skey.der -outform der

#Create a certificate request
$ openssl req -new -key rsa_skey.der -out rsa.csr
```

Test and parse PEM and DER files

* Print key information using OPENSSL

```text
# openssl rsa -in rsa_skey_t.pem -noout -text
# openssl rsa -in rsa_skey.der -noout -text
$ openssl rsa -in rsa_skey.pem -noout -text
Private-Key: (1024 bit, 2 primes)
modulus:
    00:a1:5b:8b:ae:38:e7:c8:cd:77:68:00:3c:dc:90:
    c8:69:e8:b9:a3:82:0d:3c:99:33:d8:94:92:c8:8d:
    67:9f:c6:30:f7:61:a4:26:a9:da:27:fa:e6:05:09:
    8f:1e:75:58:5d:c8:be:ab:da:26:a4:8c:04:5f:dd:
    88:78:69:73:89:d5:cd:57:0a:2c:2e:5d:de:21:cc:
    27:0f:08:a5:a1:5c:85:d2:e1:15:15:3e:c6:6a:14:
    b4:a2:ec:ca:58:7b:05:5b:dc:47:b7:17:44:ab:84:
    b0:3b:f2:50:93:54:42:ba:a8:f1:24:6f:13:48:f4:
    89:80:a0:5f:86:c2:ab:f6:5d
publicExponent: 65537 (0x10001)
privateExponent:
```

* Use asn1 dump to print key information

```text
# asn1dump.exe -in rsa_skey.pem -from PEM
$ asn1dump.exe -in rsa_skey.der -from DER
30 82 02 77                       ; SEQUENCE (631 Bytes)
   02 01                             ; INTEGER (1 Bytes)
   |  00
   30 0d                             ; SEQUENCE (13 Bytes)
      06 09                             ; OBJECT_IDENTIFIER (9 Bytes)
      |  2a 86 48 86 f7 0d 01 01 01     ;    *.H......
      05 00                             ; NULL (0 Bytes)
   04 82 02 61                       ; OCTET_STRING (609 Bytes)
   |  30 82 02 5d 02 01 00 02 81 81  ;    0..]......
   |  00 a1 5b 8b ae 38 e7 c8 cd 77  ;    ..[..8...w
```

```text
$ asn1dump.exe -in rsa_skey_t.pem -from PEM
30 82 02 5d                       ; SEQUENCE (605 Bytes)
   02 01                             ; INTEGER (1 Bytes)
   |  00
   02 81 81                          ; INTEGER (129 Bytes)
   |  00 a1 5b 8b ae 38 e7 c8 cd 77
```

Test resolution certificate request CSR

* Print CSR using OPENSSL

```text
$ openssl req -in rsa.csr -noout -text
Certificate Request:
    Data:
        Version: 1 (0x0)
        Subject: C = CN, ST = Some-State, O = Internet Widgits Pty Ltd, CN = oldprincess
        Subject Public Key Info:
            Public Key Algorithm: rsaEncryption
                Public-Key: (1024 bit)
                Modulus:
                    00:a1:5b:8b:ae:38:e7:c8:cd:77:68:00:3c:dc:90:
                    c8:69:e8:b9:a3:82:0d:3c:99:33:d8:94:92:c8:8d:
```

* Print CSR using asn1 dump

```text
$ asn1dump.exe -in rsa.csr -from PEM
30 82 01 9b                       ; SEQUENCE (411 Bytes)
   30 82 01 04                       ; SEQUENCE (260 Bytes)
      02 01                             ; INTEGER (1 Bytes)
      |  00
      30 5b                             ; SEQUENCE (91 Bytes)
         31 0b                             ; SET (11 Bytes)
```

## 3. Program with *asn1.h*

ASN1 Each node is a (tag, length, value) triplet, and ASN1 is defined in the *ASN1_NODE* type. In addition to the above contents, it also includes the *raw_data* (raw of the Data) and length *total_length*. In addition, in the relationship between ASN1 nodes, in addition to the parallel relationship at the same level, nodes such as SEQUENCE and SET also have child nodes, that is, parent-child relationships. The *nxt* pointer is the node of the same layer, and the *child* pointer is the internal node (child node) of SEQUENCE/SET.

```c
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
```

For the rest, refer to the *asn1.h* file and *demo.c*
