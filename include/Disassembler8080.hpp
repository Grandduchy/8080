#ifndef DISASSEMBLER8080_HPP
#define DISASSEMBLER8080_HPP

#include <array>

#include "State8080.hpp"

class Disassembler8080 {
public:
    using opcodePtr = void (Disassembler8080::*)(State8080&); // the type of pointer to opcode member functions

    Disassembler8080();
    void runCycle(State8080&);
    std::array<opcodePtr, 256> opcodeTable;

private:

    void unimplemented(State8080&); // an opcode that is unimplemented and will never be done
    void todo(State8080&); // an opcode that is currently unimplemented but will eventually be done

    // This cpu will only need 50 instructions in order to run invaders.
    // This cpu also does not affect the AUX flag, it's not needed for invaders, but notes will be put when it should have been set.
    // The ending extension of an opcode ex D8 or D16 refers to immediate memory; the next byte after the instruction
    // to be loaded and in for use
    // Opcodes are arranged in numeric order
    void OP_NOP(State8080&); // 0x00
    void OP_LXIB_D16(State8080&); // 0x01
    void OP_DCRB(State8080&); // 0x05
    void OP_MVIB_D8(State8080&); // 0x06
    void OP_DADB(State8080&); // 0x09
    void OP_DCRC(State8080&); // 0x0d
    void OP_MVIC_D8(State8080&); // 0x0e
    void OP_RRC(State8080&); // 0x0f
    void OP_LXID_D16(State8080&); // 0x11
    void OP_INXD(State8080&); // 0x13
    void OP_DADD(State8080&); // 0x19
    void OP_LDAXD(State8080&); // 0x1a
    void OP_LXIH_D16(State8080&); // 0x21
    void OP_INXH(State8080&); // 0x23
    void OP_MVIH_D8(State8080&); // 0x26
    void OP_DADH(State8080&); // 0x29
    void OP_LXISP_D16(State8080&); // 0x31
    void OP_STA_ADR(State8080&); // 0x32
    void OP_MVIM_D8(State8080&); // 0x36
    void OP_LDA_ADR(State8080&); // 0x3a
    void OP_MVIA_D8(State8080&); // 0x3e
    void OP_MOVD_M(State8080&); // 0x56
    void OP_MOVE_M(State8080&); // 0x5e
    void OP_MOVH_M(State8080&); // 0x66
    void OP_MOVL_A(State8080&); // 0x6f
    void OP_MOVM_A(State8080&); // 0x77
    void OP_MOVA_D(State8080&); // 0x7a
    void OP_MOVA_E(State8080&); // 0x7b
    void OP_MOVA_H(State8080&); // 0x7c
    void OP_MOVA_M(State8080&); // 0x7e
    void OP_ANAA(State8080&); // 0xa7
    void OP_XRAA(State8080&); // 0xaf
    void OP_POPB(State8080&); // 0xc1
    void OP_JNZADR(State8080&); // 0xc2
    void OP_JMPADR(State8080&); // 0xc3
    void OP_PUSHB(State8080&); // 0xc5
    void OP_ADID8(State8080&); // 0xc6
    void OP_RET(State8080&); // 0xc9
    void OP_CALLADR(State8080&); // 0xcd
    void OP_POPD(State8080&); // 0xd1
    void OP_OUTD8(State8080&); // 0xd3
    void OP_PUSHD(State8080&); //0xd5
    void OP_POPH(State8080&); // 0xe1
    void OP_PUSHH(State8080&); // 0xe5
    void OP_ANID8(State8080&); // 0xe6
    void OP_XCHG(State8080&); // 0xeb
    void OP_POPPSW(State8080&); // 0xf1
    void OP_PUSHPSW(State8080&); // 0xf5
    void OP_EI(State8080&); // 0xfb
    void OP_CPI_D8(State8080&); // 0xfe


    // functions that perform an opcode's operation using the registers themselves.
    inline void LXI_D16(State8080&, uint8_t&, uint8_t&);
    inline void DCR(State8080&, uint8_t&);
    inline void DAD(State8080&, uint8_t&, uint8_t&);
    inline void MVI_D8(State8080&, uint8_t&);
    inline void INX(uint8_t&, uint8_t&);
    inline void MOV(State8080&, uint8_t& dst, uint8_t& src);
    inline void MOV(State8080&, uint8_t& dst);// This function is only used for the H & L pair memory location as a src.
    inline void POP(State8080&, uint8_t& regPair1, uint8_t& regPair2);
    inline void PUSH(State8080&, uint8_t& regPair1, uint8_t& regPair2);
};

#endif // DISASSEMBLER8080_HPP
