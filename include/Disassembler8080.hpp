#ifndef DISASSEMBLER8080_HPP
#define DISASSEMBLER8080_HPP

#include <array>

#include "State8080.hpp"

#define EXECOPCODE(obj, ptr) ((obj).*(ptr))() // executes a pointer to a function member with the object

class Disassembler8080 {
public:
    using opcodePtr = void (Disassembler8080::*)(); // the type of pointer to opcode member functions

    Disassembler8080();
    void runCycle();
    std::array<opcodePtr, 256> opcodeTable;

    void setState(const State8080&);
    State8080 getState() const;

private:
    State8080 state;

    void unimplemented(); // an opcode that is unimplemented and will never be done
    void todo(); // an opcode that is currently unimplemented but will eventually be done

    // This cpu will only need 50 instructions in order to run invaders.
    // Opcodes are arranged in numeric order
    void OP_NOP(); // 0x00
    void OP_LXIB_D16(); // 0x01
    void OP_DCRB(); // 0x05
    void OP_MVIB_D8(); // 0x06
    void OP_DADB(); // 0x09
    void OP_DCRC(); // 0x0d
    void OP_MVIC_D8(); // 0x0e
    void OP_RRC(); // 0x0f
    void OP_LXID_D16(); // 0x11
    void OP_INXD(); // 0x13
    void OP_DADD(); // 0x19
    void OP_LDAXD(); // 0x1a
    void OP_LXIH_D16(); // 0x21
    void OP_INXH(); // 0x23
    void OP_MVIH_D8(); // 0x26
    void OP_DADH(); // 0x29
    void OP_LXISP_D16(); // 0x31
    void OP_STA_ADR(); // 0x32
    void OP_MVIM_D8(); // 0x36
    void OP_LDA_ADR(); // 0x3a
    void OP_MVIA_D8(); // 0x3e
    void OP_MOVD_M(); // 0x56
    void OP_MOVE_M(); // 0x5e
    void OP_MOVH_M(); // 0x66
    void OP_MOVL_A(); // 0x6f
    void OP_MOVM_A(); // 0x77
    void OP_MOVA_D(); // 0x7a
    void OP_MOVA_E(); // 0x7b
    void OP_MOVA_H(); // 0x7c
    void OP_MOVA_M(); // 0x7e
    void OP_ANAA(); // 0xa7
    void OP_XRAA(); // 0xaf
    void OP_POPB(); // 0xc1
    void OP_JNZADR(); // 0xc2
    void OP_JMPADR(); // 0xc3
    void OP_PUSHB(); // 0xc5
    void OP_ADID8(); // 0xc6
    void OP_RET(); // 0xc9
    void OP_CALLADR(); // 0xcd
    void OP_POPD(); // 0xd1
    void OP_OUTD8(); // 0xd3
    void OP_PUSHD(); //0xd5
    void OP_POPH(); // 0xe1
    void OP_PUSHH(); // 0xe5
    void OP_ANID8(); // 0xe6
    void OP_XCHG(); // 0xeb
    void POPPSW(); // 0xf1
    void PUSHPSW(); // 0xf5
    void EI(); // 0xfb
    void CPI_D8(); // 0xfe
};

#endif // DISASSEMBLER8080_HPP
