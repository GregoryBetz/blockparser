#ifndef __OPCODE_H__
    #define __OPCODE_H__

    #include <common.h>

    #define OPCODES                     \
        OPCODE(                  0, 0x00) \
        OPCODE(          PUSHDATA1, 0x4c) \
        OPCODE(          PUSHDATA2, 0x4d) \
        OPCODE(          PUSHDATA4, 0x4e) \
        OPCODE(            1NEGATE, 0x4f) \
        OPCODE(           RESERVED, 0x50) \
        OPCODE(                  1, 0x51) \
        OPCODE(                  2, 0x52) \
        OPCODE(                  3, 0x53) \
        OPCODE(                  4, 0x54) \
        OPCODE(                  5, 0x55) \
        OPCODE(                  6, 0x56) \
        OPCODE(                  7, 0x57) \
        OPCODE(                  8, 0x58) \
        OPCODE(                  9, 0x59) \
        OPCODE(                 10, 0x5a) \
        OPCODE(                 11, 0x5b) \
        OPCODE(                 12, 0x5c) \
        OPCODE(                 13, 0x5d) \
        OPCODE(                 14, 0x5e) \
        OPCODE(                 15, 0x5f) \
        OPCODE(                 16, 0x60) \
        OPCODE(                NOP, 0x61) \
        OPCODE(                VER, 0x62) \
        OPCODE(                 IF, 0x63) \
        OPCODE(              NOTIF, 0x64) \
        OPCODE(              VERIF, 0x65) \
        OPCODE(           VERNOTIF, 0x66) \
        OPCODE(               ELSE, 0x67) \
        OPCODE(              ENDIF, 0x68) \
        OPCODE(             VERIFY, 0x69) \
        OPCODE(             RETURN, 0x6a) \
        OPCODE(         TOALTSTACK, 0x6b) \
        OPCODE(       FROMALTSTACK, 0x6c) \
        OPCODE(              2DROP, 0x6d) \
        OPCODE(               2DUP, 0x6e) \
        OPCODE(               3DUP, 0x6f) \
        OPCODE(              2OVER, 0x70) \
        OPCODE(               2ROT, 0x71) \
        OPCODE(              2SWAP, 0x72) \
        OPCODE(              IFDUP, 0x73) \
        OPCODE(              DEPTH, 0x74) \
        OPCODE(               DROP, 0x75) \
        OPCODE(                DUP, 0x76) \
        OPCODE(                NIP, 0x77) \
        OPCODE(               OVER, 0x78) \
        OPCODE(               PICK, 0x79) \
        OPCODE(               ROLL, 0x7a) \
        OPCODE(                ROT, 0x7b) \
        OPCODE(               SWAP, 0x7c) \
        OPCODE(               TUCK, 0x7d) \
        OPCODE(                CAT, 0x7e) \
        OPCODE(             SUBSTR, 0x7f) \
        OPCODE(               LEFT, 0x80) \
        OPCODE(              RIGHT, 0x81) \
        OPCODE(               SIZE, 0x82) \
        OPCODE(             INVERT, 0x83) \
        OPCODE(                AND, 0x84) \
        OPCODE(                 OR, 0x85) \
        OPCODE(                XOR, 0x86) \
        OPCODE(              EQUAL, 0x87) \
        OPCODE(        EQUALVERIFY, 0x88) \
        OPCODE(          RESERVED1, 0x89) \
        OPCODE(          RESERVED2, 0x8a) \
        OPCODE(               1ADD, 0x8b) \
        OPCODE(               1SUB, 0x8c) \
        OPCODE(               2MUL, 0x8d) \
        OPCODE(               2DIV, 0x8e) \
        OPCODE(             NEGATE, 0x8f) \
        OPCODE(                ABS, 0x90) \
        OPCODE(                NOT, 0x91) \
        OPCODE(          0NOTEQUAL, 0x92) \
        OPCODE(                ADD, 0x93) \
        OPCODE(                SUB, 0x94) \
        OPCODE(                MUL, 0x95) \
        OPCODE(                DIV, 0x96) \
        OPCODE(                MOD, 0x97) \
        OPCODE(             LSHIFT, 0x98) \
        OPCODE(             RSHIFT, 0x99) \
        OPCODE(            BOOLAND, 0x9a) \
        OPCODE(             BOOLOR, 0x9b) \
        OPCODE(           NUMEQUAL, 0x9c) \
        OPCODE(     NUMEQUALVERIFY, 0x9d) \
        OPCODE(        NUMNOTEQUAL, 0x9e) \
        OPCODE(           LESSTHAN, 0x9f) \
        OPCODE(        GREATERTHAN, 0xa0) \
        OPCODE(    LESSTHANOREQUAL, 0xa1) \
        OPCODE( GREATERTHANOREQUAL, 0xa2) \
        OPCODE(                MIN, 0xa3) \
        OPCODE(                MAX, 0xa4) \
        OPCODE(             WITHIN, 0xa5) \
        OPCODE(          RIPEMD160, 0xa6) \
        OPCODE(               SHA1, 0xa7) \
        OPCODE(             SHA256, 0xa8) \
        OPCODE(            HASH160, 0xa9) \
        OPCODE(            HASH256, 0xaa) \
        OPCODE(      CODESEPARATOR, 0xab) \
        OPCODE(           CHECKSIG, 0xac) \
        OPCODE(     CHECKSIGVERIFY, 0xad) \
        OPCODE(      CHECKMULTISIG, 0xae) \
        OPCODE(CHECKMULTISIGVERIFY, 0xaf) \
        OPCODE(               NOP1, 0xb0) \
        OPCODE(               NOP2, 0xb1) \
        OPCODE(               NOP3, 0xb2) \
        OPCODE(               NOP4, 0xb3) \
        OPCODE(               NOP5, 0xb4) \
        OPCODE(               NOP6, 0xb5) \
        OPCODE(               NOP7, 0xb6) \
        OPCODE(               NOP8, 0xb7) \
        OPCODE(               NOP9, 0xb8) \
        OPCODE(              NOP10, 0xb9) \
        OPCODE(       SMALLINTEGER, 0xfa) \
        OPCODE(            PUBKEYS, 0xfb) \
        OPCODE(         PUBKEYHASH, 0xfd) \
        OPCODE(             PUBKEY, 0xfe) \
        OPCODE(      INVALIDOPCODE, 0xff) \

    #define OPCODE(x, y) kOP_##x = 0x##y,
        enum Opcode {
            OPCODES
            kOP_BOGUS_TO_GET_GCC_TO_STFU
        };
    #undef OPCODE

    const char *getOpcodeName(uint8_t op);

#endif // __OPCODE_H__

