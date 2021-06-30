#include "Instruction.hpp"

#define RET(mnemonic) return std::make_shared<mnemonic>(encoding, pc, reg)
#define CASE(mnemonic, type) case mnemonic::type: RET(mnemonic)

auto Instruction::Decode(const u32 encoding, const u32 pc, const u32 reg[32])
    -> InstPtr {
    switch (GetOpcode(encoding)) {
    case ALU_ri::opcode:
        switch (GetFunct3(encoding)) {
            CASE(ADDI,  funct3);
            CASE(SLTI,  funct3);
            CASE(SLTIU, funct3);
            CASE(ANDI,  funct3);
            CASE(ORI,   funct3);
            CASE(XORI,  funct3);
            CASE(SLLI,  funct3);
        case 0b101:
            if (getbits<30>(encoding)) RET(SRLI);
            else RET(SRAI);
        }
    CASE(LUI,   opcode);
    CASE(AUIPC, opcode);
    case ALU_rr::opcode:
        switch (GetFunct3(encoding)) {
            CASE(SLT,  funct3);
            CASE(SLTU, funct3);
            CASE(AND,  funct3);
            CASE(OR,   funct3);
            CASE(XOR,  funct3);
            CASE(SLL,  funct3);
        case 0b000:
            switch (GetFunct7(encoding)) {
                CASE(ADD, funct7);
                CASE(SUB, funct7);
            }
        case 0b101:
            switch (GetFunct7(encoding)) {
                CASE(SRL, funct7);
                CASE(SRA, funct7);
            }
        }
    CASE(JAL,  opcode);
    CASE(JALR, opcode);
    case BranchCC_rri::opcode:
        switch (GetFunct3(encoding)) {
            CASE(BEQ,  funct3);
            CASE(BNE,  funct3);
            CASE(BLT,  funct3);
            CASE(BLTU, funct3);
            CASE(BGE,  funct3);
            CASE(BGEU, funct3);
        }
    case Load_ri::opcode:
        switch (GetFunct3(encoding)) {
            CASE(LB,  funct3);
            CASE(LH,  funct3);
            CASE(LW,  funct3);
            CASE(LBU, funct3);
            CASE(LHU, funct3);
        }
    case Store_rri::opcode:
        switch (GetFunct3(encoding)) {
            CASE(SB, funct3);
            CASE(SH, funct3);
            CASE(SW, funct3);
        }
    }
    if constexpr (not NOASSERT)
        assert(false && "no matching encoding");
    return nullptr;
}
