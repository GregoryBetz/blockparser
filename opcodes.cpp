
#include <opcodes.h>

const char *getOpcodeName(
    uint8_t op
)
{
    #define OPCODE(x, y) if((y)==op) return "OP_" #x;
        OPCODES
    #undef OPCODE
    return "OP_UNKNOWN";
}

