# ASN1DUMP

[English README file](README.EN.md)

asn1打印工具，支持解析PEM或DER格式的数据，并输出以ASN1格式。

关于ASN1的基础知识可参考Miscrosoft的 [相关文档](https://learn.microsoft.com/en-us/windows/win32/seccertenroll/distinguished-encoding-rules)

## 1. 快速开始

```text
│  asn1.h       外部接口头文件
│  appmain.c    APP应用程序(控制台)
│  demo.c       样例代码
│  main.c       主函数
│
└─lib           代码目录
        asn1_der.c      ASN1与DER/PEM转化
        asn1_dump.c     打印ASN1数据
        asn1_inner.c    ASN1函数
        asn1_inner.h    ASN1函数
        mString.c       字符串处理函数
        mString.h       字符串处理函数
```

编译样例

```text
gcc -o asn1dump.exe *.c lib/*.c
```

命令行参数

```text
asn1dump -in <input file> -from <PEM|DER>
```

## 2. 运行效果/测试

**注**：本节测试所使用的文件均已放置在*test*目录下。

使用OPENSSL生成PEM和DER格式文件

```text
# 生成1024比特的RSA密钥，PEM格式 
$ openssl genrsa -out rsa_skey.pem 1024

# 将密钥转化为传统(traditional)格式
$ openssl rsa -in rsa_skey.pem -out rsa_skey_t.pem -outform pem -traditional

# 将PEM格式密钥转化为DER格式
$ openssl rsa -in rsa_skey.pem -out rsa_skey.der -outform der

# 创建一个证书请求
$ openssl req -new -key rsa_skey.der -out rsa.csr
```

测试解析PEM和DER文件

* 使用OPENSSL打印密钥信息

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

* 使用asn1dump打印密钥信息

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

测试解析证书请求CSR

* 使用OPENSSL打印CSR

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

* 使用asn1dump打印CSR

```text
$ asn1dump.exe -in rsa.csr -from PEM
30 82 01 9b                       ; SEQUENCE (411 Bytes)
   30 82 01 04                       ; SEQUENCE (260 Bytes)
      02 01                             ; INTEGER (1 Bytes)
      |  00
      30 5b                             ; SEQUENCE (91 Bytes)
         31 0b                             ; SET (11 Bytes)
```

## 3. 使用asn1.h编程

ASN1每个节点为tag、length、value三元组，代码中定义ASN1_NODE类型，除了上述内容外，还包括该节点原始数据raw_data与长度total_length。此外，ASN1节点与节点间的关系，除了处于同一层的并列关系外，SEQUENCE和SET这类节点还存在孩子节点，即还有父子关系。nxt指针为同一层的节点，child指针为SEQUENCE/SET的内部节点(孩子节点)。

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

其余的内容可参考asn1.h文件与demo.c
